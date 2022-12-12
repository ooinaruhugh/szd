#include <ios>
#include <iomanip>

#include <stdexcept>

#include "zip.h"

using namespace std;

ZipFile::ZipFile(const char *filename) {
    file = make_shared<ifstream>(filename, ifstream::ate | ifstream::binary);

    // TODO: Handle file errors
    eocdrPos = findEOCDR();
    // TODO: Handle "no eocdr at all"
    eocdr = readEOCDR(eocdrPos);
    // TODO: Handle "bad eocdr" and to what extent
    cdr = readCDRs(eocdr.startOfCDR, eocdr.currentDiskEntriesTotal);
    // TODO: Are there any errors to handle?
    entries = getZipEntries(cdr);
}

// ZipFile::ZipFile(const ZipFile& zipfile) {

// }

std::streampos ZipFile::findEOCDR() {
    // TODO Handle random occurences of 0x06054B50 https://stackoverflow.com/questions/8593904/how-to-find-the-position-of-central-directory-in-a-zip-file)
    std::streampos central_directory = file->tellg();;
    
    const int readBufferSize = 512;
    std::array<char,readBufferSize> searchBuffer;

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
    
    return needle ? central_directory + (needle - searchBuffer.data()) : std::streampos(-1);
}

EOCDR ZipFile::readEOCDR(std::streampos at) {
    unsigned char buffer[eocdrSize];
    file->clear();
    file->seekg(at, std::ios_base::beg);
    file->read(reinterpret_cast<char*>(buffer), eocdrSize);

    if (getDWordLE(buffer) != eocdrMagic) {
        throw std::invalid_argument("Specified streampos doesn't point to an end of central directory record.");
    }

    EOCDR eocdr{
        .numOfDisk = getWordLE(buffer+4),
        .numOfStartDisk = getWordLE(buffer+6),
        .currentDiskEntriesTotal = getWordLE(buffer+8),
        .entriesTotal = getWordLE(buffer+10),
        .size = getDWordLE(buffer+12),
        .startOfCDR = getDWordLE(buffer+16),
        .commentSize = getWordLE(buffer+20),
    };

    if (eocdr.commentSize > 0) {
        eocdr.comment.resize(eocdr.commentSize);
        file->read(eocdr.comment.data(), eocdr.commentSize);
    }

    return eocdr;
}

std::vector<CDR> ZipFile::readCDRs(std::streampos beginAt, WORD noOfRecords) {
    unsigned char buffer[cdrSize];

    file->clear();
    file->seekg(beginAt, std::ios_base::beg);

    while (noOfRecords > 0) {
        auto at = file->tellg();
        file->read(reinterpret_cast<char*>(buffer), cdrSize);

        if (getDWordLE(buffer) != cdrMagic) {
            std::stringstream errMsg;
            errMsg << "Encountered a non-central directory record at "
                   << std::hex << at;
            throw std::runtime_error(errMsg.str());
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
            .filenameLength = getWordLE(buffer+28),
            .extraLength = getWordLE(buffer+30),
            .commentLength = getWordLE(buffer+32),
            .diskNoStart = getWordLE(buffer+34),
            .internalAttr = getWordLE(buffer+36),
            .externalAttr = getDWordLE(buffer+38),
            .relOffset = getDWordLE(buffer+42)
        };

        record.filename.resize(record.filenameLength);
        file->read(record.filename.data(), record.filenameLength);

        record.extra.resize(record.extraLength);
        file->read(record.extra.data(), record.extraLength);

        record.comment.resize(record.commentLength);
        file->read(record.comment.data(), record.commentLength);

        cdr.emplace_back(record);

        noOfRecords--;
    } 

    return cdr;

}

std::vector<ZipEntry> ZipFile::getZipEntries(std::vector<CDR> cdrs) {
    using namespace std;
    std::vector<ZipEntry> entries;

    for (auto cdr : cdrs) {
        auto localHeader = LocalHeader::readLocalHeader(*file, cdr.relOffset);

        entries.emplace_back(localHeader, cdr);
    }

    return entries;
}

void ZipFile::copyNBytesTo(std::ofstream& outfile, size_t n, char* buffer, size_t n_buffer) {
    while (n > n_buffer) {
        file->read(buffer, n_buffer);
        outfile.write(buffer, n_buffer);

        n -= n_buffer;
    } if (n > 0) {
        file->read(buffer, n);
        outfile.write(buffer, n);
    }
}

void ZipFile::copyNBytesAtTo(std::ofstream& outfile, std::streampos at, size_t n, char* buffer, size_t n_buffer) {
    file->seekg(at);
    copyNBytesTo(outfile, n, buffer, n_buffer);
}

std::ofstream& operator<< (std::ofstream& os, ZipFile zipfile) {
    const size_t blockSize = 4096;
    // Buffer for copying unchanged file contents
    char buffer[blockSize];

    auto entries = zipfile.entries;

    // Write out all the local file headers plus data payloads (the ones that we've collected)
    for (auto& entry : entries) {
            auto header = entry.localHeader;

            auto pos = os.tellp();
            entry.cdr.relOffset = pos;

            os.write(reinterpret_cast<const char*>(&localHeaderMagic), sizeof(localHeaderMagic));
            os.write(header.getAsByteArray().data(), localHeaderSize-4);
            os.write(header.filename.data(), header.filenameLength);
            os.write(header.extra.data(), header.extraLength);
            
            zipfile.copyNBytesTo(os, header.compressedSize, buffer, blockSize);
    }

    // TODO: Write archive extra data if there's any
    // Write updated CDR

    auto cdrPos = os.tellp();
    for (auto entry : entries) {
        auto cdr = entry.cdr;

        os.write(reinterpret_cast<const char*>(&cdrMagic), sizeof(cdrMagic));
        os.write(cdr.getAsByteArray().data(), cdrSize-4);
        os.write(cdr.filename.data(), cdr.filenameLength);
        os.write(cdr.extra.data(), cdr.extraLength);
        os.write(cdr.comment.data(), cdr.commentLength);
    }

    zipfile.eocdr.startOfCDR = cdrPos;

    // Write updated EOCDR
    os.write(reinterpret_cast<const char*>(&eocdrMagic), sizeof(eocdrMagic));
    os.write(zipfile.eocdr.getAsByteArray().data(), eocdrSize-4);
    if (zipfile.eocdr.commentSize > 0) 
        os.write(zipfile.eocdr.comment.data(), zipfile.eocdr.commentSize);

    return os;
}