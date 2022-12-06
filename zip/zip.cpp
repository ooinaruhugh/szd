#include <ios>
#include <iomanip>

#include <stdexcept>

#include "zip.h"

Zipfile::Zipfile(const char *filename) {
    this->file = std::ifstream(filename, std::ifstream::ate | std::ifstream::binary);
}

std::ostream& operator<< (std::ostream& os, EOCDR eocdr) {
    auto f(os.flags());

    os << "number of this disk:\t" 
       << std::dec << eocdr.numOfDisk << std::endl;
    os << "number of the disk with the\nstart of the central directory:\t"
       << eocdr.numOfStartDisk << std::endl;
    os << "total number of entries in the\ncentral directory on this disk:\t"
       << eocdr.currentDiskEntriesTotal << std::endl;
    os << "total number of entries in\nthe central directory:\t"
       << eocdr.entriesTotal << std::endl;
    os << "size of the central directory:\t0x"
       << std::hex << std::setfill('0') << std::setw(2) << eocdr.size << std::endl;
    os << "offset of start of central\ndirectory with respect to\nthe starting disk number:\t0x"
       << eocdr.startOfCDR << std::endl;
    os << ".ZIP file comment length:\t0x"
       << eocdr.commentSize << std::endl;
    os << ".ZIP file comment:\t"
       << "<TODO: Here goes comment>" << std::endl;

    os.flags(f);
    return os;
}

std::ostream& operator<< (std::ostream& os, CDR record) {
    auto f(os.flags());

    os << "filename: " << std::string(record.filename.begin(), record.filename.end()) << "🔚" << std::endl;
    os << "offset: " << std::hex << record.relOffset << std::endl;

    // os << "version made by:\t" 
    //    << std::dec << record.versionMadeBy << std::endl;
    // os << "version needed to extract:\t" 
    //    << std::dec << record.versionNeeded << std::endl;
    os << "file name length:\t0x"
       << std::hex << std::setfill('0') << std::setw(2) << record.filenameLength << std::endl;
    os << "extra field length:\t0x"
       << record.extraLength << std::endl;
    os << "file comment length:\t0x"
       << record.commentLength << std::endl;

    os.flags(f);
    return os;
}

std::ostream& operator<< (std::ostream& os, LocalHeader record) {
    auto f(os.flags());

    os << "filename: " << std::string(record.filename.begin(), record.filename.end()) << "🔚" << std::endl;
    os << "compressed (actual, right-now) size: " << std::hex << record.compressedSize << std::endl;

    os << "file name length:\t0x"
       << std::hex << std::setfill('0') << std::setw(2) << record.filenameLength << std::endl;
    os << "extra field length:\t0x"
       << record.extraLength << std::endl; 

    os.flags(f);
    return os;
}

std::streampos Zipfile::findEOCDR() {
    // TODO Handle random occurences of 0x06054B50 https://stackoverflow.com/questions/8593904/how-to-find-the-position-of-central-directory-in-a-zip-file)
    std::streampos central_directory;
    
    const int readBufferSize = 512;
    // char searchBuffer[readBufferSize+1];
    std::array<char,readBufferSize> searchBuffer;
    // searchBuffer.resize(readBufferSize+1);

    char* needle = nullptr;

    central_directory = this->file.tellg();

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
    
    return needle ? central_directory + (needle - searchBuffer.data()) : std::streampos(-1);
}

EOCDR Zipfile::readEOCDR(std::streampos at) {
    unsigned char buffer[eocdrSize];
    this->file.clear();
    this->file.seekg(at, std::ios_base::beg);

    // std::cout << at << " " << zipfile.tellg() << std::endl;
    this->file.read(reinterpret_cast<char*>(buffer), eocdrSize);

    if (readDWordLE(buffer) != eocdrMagic) {
        throw std::invalid_argument("Specified streampos doesn't point to an end of central directory record.");
    }

    EOCDR eocdr{
        .numOfDisk = readWordLE(buffer+4),
        .numOfStartDisk = readWordLE(buffer+6),
        .currentDiskEntriesTotal = readWordLE(buffer+8),
        .entriesTotal = readWordLE(buffer+10),
        .size = readDWordLE(buffer+12),
        .startOfCDR = readDWordLE(buffer+16),
        .commentSize = readWordLE(buffer+20),
        .comment = NULL
    };

    if (eocdr.commentSize > 0) {

    }

    // for (unsigned char c : buffer) {
    //     auto f{std::cout.flags()};

    //     std::cout << std::setfill('0') << std::setw(2) << std::hex << +c << ' ';
        
    //     std::cout.flags(f);
    // }
    // std::cout << std::endl;

    return eocdr;
}

std::vector<CDR> Zipfile::readCDR( std::streampos beginAt, uint16_t noOfRecords) {
    unsigned char buffer[cdrSize];
    this->file.clear();
    this->file.seekg(beginAt, std::ios_base::beg);

    std::vector<CDR> cdr;
    while (noOfRecords > 0) {
        auto at = this->file.tellg();
        this->file.read(reinterpret_cast<char*>(buffer), cdrSize);

        if (readDWordLE(buffer) != cdrMagic) {
            std::stringstream errMsg;
            errMsg << "Encountered a non-central directory record at "
                   << std::hex << at;
            throw std::runtime_error(errMsg.str());
        }

        CDR record{
            .versionMadeBy = readWordLE(buffer+4),
            .versionNeeded = readWordLE(buffer+6),
            .generalPurpose = readWordLE(buffer+8),
            .compressionMethod = readWordLE(buffer+10),
            .lastModTime = readWordLE(buffer+12),
            .lastModDate = readWordLE(buffer+14),
            .crc32 = readDWordLE(buffer+16),
            .compressedSize = readDWordLE(buffer+20),
            .uncompressedSize = readDWordLE(buffer+24),
            .filenameLength = readWordLE(buffer+28),
            .extraLength = readWordLE(buffer+30),
            .commentLength = readWordLE(buffer+32),
            .diskNoStart = readWordLE(buffer+34),
            .internalAttr = readWordLE(buffer+36),
            .externalAttr = readDWordLE(buffer+38),
            .relOffset = readDWordLE(buffer+42)
        };

        record.filename.resize(record.filenameLength);
        this->file.read(record.filename.data(), record.filenameLength);

        record.extra.resize(record.extraLength);
        this->file.read(record.extra.data(), record.extraLength);

        record.comment.resize(record.commentLength);
        this->file.read(record.comment.data(), record.commentLength);

        cdr.emplace_back(record);

        noOfRecords--;
    } 

    return cdr;
}

std::vector<LocalHeader> Zipfile::readLocalHeaders(std::vector<CDR> cdr) {
    unsigned char buffer[localHeaderSize];
    this->file.clear();

    std::vector<LocalHeader> localHeaders;
    for (CDR record : cdr) {
        auto at = this->file.tellg();
        // std::cout << at << std::endl;

        this->file.seekg(record.relOffset, std::ios_base::beg);
        this->file.read(reinterpret_cast<char*>(buffer), localHeaderSize);

        if (readDWordLE(buffer) != localHeaderMagic) {
            std::stringstream errMsg;
            errMsg << "Encountered a non-local header at "
                   << std::hex << at;
            throw std::runtime_error(errMsg.str());
        }

        LocalHeader localHeader{
            .versionNeeded = readWordLE(buffer+4),
            .generalPurpose = readWordLE(buffer+6),
            .compressionMethod = readWordLE(buffer+8),
            .lastModTime = readWordLE(buffer+10),
            .lastModDate = readWordLE(buffer+12),
            .crc32 = readDWordLE(buffer+14),
            .compressedSize = readDWordLE(buffer+18),
            .uncompressedSize = readDWordLE(buffer+22),
            .filenameLength = readWordLE(buffer+26),
            .extraLength = readWordLE(buffer+28)
        };
        localHeader.data = this->file.tellg() + std::streamoff(localHeader.filenameLength + localHeader.extraLength);

        localHeader.filename.resize(record.filenameLength);
        this->file.read(localHeader.filename.data(), localHeader.filenameLength);

        localHeader.extra.resize(localHeader.extraLength);
        this->file.read(localHeader.extra.data(), localHeader.extraLength);

        localHeaders.emplace_back(localHeader);
    }

    return localHeaders;
}

std::vector<char> LocalHeader::getAsByteArray() {
    std::vector<char> zaBytes;
    zaBytes.reserve(localHeaderSize);

    appendVectorToVector(zaBytes, writeWordLE(this->versionNeeded));
    printByteBuffer(zaBytes);
    appendVectorToVector(zaBytes, writeWordLE(this->generalPurpose));
    printByteBuffer(zaBytes);
    appendVectorToVector(zaBytes, writeWordLE(this->compressionMethod));
    printByteBuffer(zaBytes);
    appendVectorToVector(zaBytes, writeWordLE(this->lastModTime));
    printByteBuffer(zaBytes);
    appendVectorToVector(zaBytes, writeWordLE(this->lastModDate));
    printByteBuffer(zaBytes);
    appendVectorToVector(zaBytes, writeDWordLE(this->crc32));
    printByteBuffer(zaBytes);
    appendVectorToVector(zaBytes, writeDWordLE(this->compressedSize));
    printByteBuffer(zaBytes);
    appendVectorToVector(zaBytes, writeDWordLE(this->uncompressedSize));
    printByteBuffer(zaBytes);
    appendVectorToVector(zaBytes, writeWordLE(this->filenameLength));
    printByteBuffer(zaBytes);
    appendVectorToVector(zaBytes, writeWordLE(this->extraLength));
    printByteBuffer(zaBytes);

    std::cout << zaBytes.size() << std::endl;
    

    return zaBytes;
}