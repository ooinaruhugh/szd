#include <ios>
#include <iomanip>

#include <stdexcept>

#include "zip.h"

ZipFile::ZipFile(const char *filename) {
    this->file = std::ifstream(filename, std::ifstream::ate | std::ifstream::binary);
}

std::streampos ZipFile::findEOCDR() {
    if (this->hasEocdrPos) return this->eocdrPos;

    // TODO Handle random occurences of 0x06054B50 https://stackoverflow.com/questions/8593904/how-to-find-the-position-of-central-directory-in-a-zip-file)
    std::streampos central_directory = this->file.tellg();;
    
    const int readBufferSize = 512;
    std::array<char,readBufferSize> searchBuffer;

    char* needle = nullptr;

    do
    {
        if (central_directory > readBufferSize) 
            central_directory -= readBufferSize;
        else 
            central_directory = 0;
        
        this->file.seekg(central_directory);

        this->file.read(searchBuffer.data(), readBufferSize);

        needle = (char*)memmem(searchBuffer.data(), readBufferSize, &eocdrMagic, 4);
    } while ((central_directory > 0) && needle == nullptr);
    
    this->eocdrPos = needle ? central_directory + (needle - searchBuffer.data()) : std::streampos(-1);
    this->hasEocdrPos = true;

    return this->eocdrPos;
}

EOCDR ZipFile::readEOCDR(std::streampos at) {
    unsigned char buffer[eocdrSize];
    this->file.clear();
    this->file.seekg(at, std::ios_base::beg);
    this->file.read(reinterpret_cast<char*>(buffer), eocdrSize);

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
        this->file.read(eocdr.comment.data(), eocdr.commentSize);
    }

    this->eocdr = eocdr;
    this->hasEocdr = true;

    return eocdr;
}

EOCDR ZipFile::readEOCDR() {
    if (this->hasEocdr) return this->eocdr;
    else if (this->hasEocdrPos) return this->readEOCDR(this->eocdrPos);
    else {
        auto at = this->eocdrPos;
        if (at == -1) {
            throw std::invalid_argument("The given file is not a zipfile.");
        }

        return this->readEOCDR(at);
    }
}

std::vector<CDR> ZipFile::readCDRs(std::streampos beginAt, WORD noOfRecords) {
    unsigned char buffer[cdrSize];

    this->file.clear();
    this->file.seekg(beginAt, std::ios_base::beg);

    while (noOfRecords > 0) {
        auto at = this->file.tellg();
        this->file.read(reinterpret_cast<char*>(buffer), cdrSize);

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
        this->file.read(record.filename.data(), record.filenameLength);

        record.extra.resize(record.extraLength);
        this->file.read(record.extra.data(), record.extraLength);

        record.comment.resize(record.commentLength);
        this->file.read(record.comment.data(), record.commentLength);

        this->cdr.emplace_back(record);

        noOfRecords--;
    } 

    this->hasCDR = true;

    return this->cdr;
}

std::vector<CDR> ZipFile::readCDRs() {
    return this->cdr;
}


std::vector<ZipEntry> ZipFile::getZipEntries(std::vector<CDR> cdrs) {
    using namespace std;
    std::vector<ZipEntry> entries;

    for (auto cdr : cdrs) {
        auto localHeader = LocalHeader::readLocalHeader(this->file, cdr.relOffset);

        entries.emplace_back(localHeader, cdr);
    }

    return entries;
}

void ZipFile::copyNBytesTo(std::ofstream& outfile, size_t n, char* buffer, size_t n_buffer) {
    while (n > n_buffer) {
        this->file.read(buffer, n_buffer);
        outfile.write(buffer, n_buffer);

        n -= n_buffer;
    } if (n > 0) {
        this->file.read(buffer, n);
        outfile.write(buffer, n);
    }
}

void ZipFile::updateOffsets(std::streamoff offset) {
    for (auto& cdr : this->cdr) {
        cdr.relOffset += offset;
    }

    eocdr.startOfCDR += offset;
}