#include <ios>
#include <iomanip>
#include <filesystem>

#include <stdexcept>

#include "zip.h"

using namespace std;

ZipFile::ZipFile(const char *filename) {
    if (!filesystem::exists(filename)) {
        stringstream errMsg;
        errMsg << "Zipfile "
                << filename << " does not exist.";
        throw invalid_argument(errMsg.str());
    }

    file = make_shared<ifstream>(filename, ifstream::ate | ifstream::binary);

    if (!file->good()) {
        throw ifstream::failure("Failed to open zipfile.");
    }

    eocdrPos = findEOCDR();
    if (eocdrPos == -1) {
        throw invalid_argument("File specified does not contain a zipfile.");
    }

    eocdr = readEOCDR(eocdrPos);
    cdr = readCDRs(eocdr.startOfCDR, eocdr.currentDiskEntriesTotal);
    // TODO: Are there any errors to handle?
    entries = getZipEntries(cdr);
}

ZipFile::ZipFile(const string& zipfile) : ZipFile::ZipFile(zipfile.data()) {}

streampos ZipFile::findEOCDR() {
    // TODO Handle random occurences of 0x06054B50 https://stackoverflow.com/questions/8593904/how-to-find-the-position-of-central-directory-in-a-zip-file)
    streampos central_directory = file->tellg();
    
    const int readBufferSize = 512;
    array<char,readBufferSize> searchBuffer;

    char* needle = nullptr;

    do
    {
        if (central_directory > readBufferSize) 
            central_directory -= readBufferSize;
        else 
            central_directory = 0;
        
        file->seekg(central_directory);

        file->read(searchBuffer.data(), readBufferSize);

        needle = (char*)memmem(searchBuffer.data(), readBufferSize, &eocdrMagic, 4);
    } while ((central_directory > 0) && needle == nullptr);
    
    return needle ? central_directory + (needle - searchBuffer.data()) : streampos(-1);
}

EOCDR ZipFile::readEOCDR(streampos at) {
    unsigned char buffer[eocdrSize];
    file->clear();
    file->seekg(at, file->beg);
    file->read(reinterpret_cast<char*>(buffer), eocdrSize);

    if (getDWordLE(buffer) != eocdrMagic) {
        throw invalid_argument("There is not an end of central directory record at the specified position.");
    }

    EOCDR eocdr{
        .numOfDisk = getWordLE(buffer+4),
        .numOfStartDisk = getWordLE(buffer+6),
        .currentDiskEntriesTotal = getWordLE(buffer+8),
        .entriesTotal = getWordLE(buffer+10),
        .size = getDWordLE(buffer+12),
        .startOfCDR = getDWordLE(buffer+16)
    };

    auto commentSize = getWordLE(buffer+20);

    if (commentSize > 0) {
        eocdr.comment.resize(commentSize);
        file->read(eocdr.comment.data(), commentSize);
    } else {
        eocdr.comment = vector<char>(0, '.');
    }

    return eocdr;
}

vector<CDR> ZipFile::readCDRs(streampos beginAt, WORD noOfRecords) {
    unsigned char buffer[cdrSize];

    file->clear();
    file->seekg(beginAt, file->beg);

    while (noOfRecords > 0) {
        auto at = file->tellg();
        file->read(reinterpret_cast<char*>(buffer), cdrSize);

        if (getDWordLE(buffer) != cdrMagic) {
            stringstream errMsg;
            errMsg << "Encountered a non-central directory record at "
                   << hex << at;
            throw runtime_error(errMsg.str());
        }

        CDR record{
            .versionMadeBy = getWordLE(buffer+4),
            .versionNeeded = getWordLE(buffer+6),
            .generalPurpose = getWordLE(buffer+8),
            .compressionMethod = getWordLE(buffer+10),
            .lastModTime = getWordLE(buffer+12),
            .lastModDate = getWordLE(buffer+14),
            .crc32 = getDWordLE(buffer+16),
            .compressedSize = getDWordLE(buffer+20),
            .uncompressedSize = getDWordLE(buffer+24),
            .diskNoStart = getWordLE(buffer+34),
            .internalAttr = getWordLE(buffer+36),
            .externalAttr = getDWordLE(buffer+38),
            .relOffset = getDWordLE(buffer+42)
        };

        auto filenameLength = getWordLE(buffer+28);
        auto extraLength = getWordLE(buffer+30);
        auto commentLength = getWordLE(buffer+32);

        record.filename.resize(filenameLength);
        file->read(record.filename.data(), filenameLength);

        record.extra.resize(extraLength);
        file->read(record.extra.data(), extraLength);

        record.comment.resize(commentLength);
        file->read(record.comment.data(), commentLength);

        cdr.emplace_back(record);

        noOfRecords--;
    } 

    return cdr;

}

vector<ZipEntry> ZipFile::getZipEntries(vector<CDR> cdrs) {
    vector<ZipEntry> entries;

    for (auto &&cdr : cdrs) {
        auto localHeader = LocalHeader::readLocalHeader(*file, cdr.relOffset);

        entries.emplace_back(localHeader, cdr);

        if (localHeader.hasDataDescriptor()) {
            // TODO: Obviously
        }
    }

    return entries;
}

void ZipFile::copyNBytesTo(ofstream& outfile, size_t n, char* buffer, size_t n_buffer) {
    while (n > n_buffer) {
        file->read(buffer, n_buffer);
        outfile.write(buffer, n_buffer);

        n -= n_buffer;
    } if (n > 0) {
        file->read(buffer, n);
        outfile.write(buffer, n);
    }
}

void ZipFile::copyNBytesAtTo(ofstream& outfile, streampos at, size_t n, char* buffer, size_t n_buffer) {
    file->seekg(at);
    copyNBytesTo(outfile, n, buffer, n_buffer);
}

ofstream& operator<< (ofstream& os, ZipFile zipfile) {
    const size_t blockSize = 4096;
    // Buffer for copying unchanged file contents
    char buffer[blockSize];

    auto entries = zipfile.entries;

    // Write out all the local file headers plus data payloads (the ones that we've collected)
    for (auto& entry : entries) {
            auto header = entry.localHeader;

            auto whereTheDataIs = entry.cdr->relOffset 
                                + localHeaderSize 
                                + header.filenameLength() 
                                + header.extraLength();

            auto pos = os.tellp();
            entry.cdr->relOffset = pos;

            os.write(reinterpret_cast<const char*>(&localHeaderMagic), sizeof(localHeaderMagic));
            os.write(header.getAsByteArray().data(), localHeaderSize-4);
            os.write(header.filename.data(), header.filenameLength());
            os.write(header.extra.data(), header.extraLength());
            
            zipfile.copyNBytesAtTo(os, whereTheDataIs, header.compressedSize, buffer, blockSize);
    }

    // TODO: Write archive extra data if there's any
    // Write updated CDR

    auto cdrPos = os.tellp();
    for (auto entry : entries) {
        auto cdr = entry.cdr;

        os.write(reinterpret_cast<const char*>(&cdrMagic), sizeof(cdrMagic));
        os.write(cdr->getAsByteArray().data(), cdrSize-4);
        os.write(cdr->filename.data(), cdr->filenameLength());
        os.write(cdr->extra.data(), cdr->extraLength());
        os.write(cdr->comment.data(), cdr->commentLength());
    }

    zipfile.eocdr.startOfCDR = cdrPos;

    // Write updated EOCDR
    os.write(reinterpret_cast<const char*>(&eocdrMagic), sizeof(eocdrMagic));
    os.write(zipfile.eocdr.getAsByteArray().data(), eocdrSize-4);
    if (zipfile.eocdr.commentSize() > 0) 
        os.write(zipfile.eocdr.comment.data(), zipfile.eocdr.commentSize());

    return os;
}