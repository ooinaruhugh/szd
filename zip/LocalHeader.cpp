#include "LocalHeader.h"
#include <iomanip>
#include <fstream>
using namespace std;

std::ostream& operator<< (std::ostream& os, LocalHeader record) {
    auto f(os.flags());

    os << "filename: " << string(record.filename.begin(), record.filename.end()) << "🔚" << endl;
    os << "compressed (actual, right-now) size: " << hex << record.compressedSize << endl;

    os << "file name length:\t0x"
       << hex << setfill('0') << std::setw(2) << record.filenameLength << endl;
    os << "extra field length:\t0x"
       << record.extraLength << endl; 

    os.flags(f);
    return os;
}

std::vector<char> LocalHeader::getAsByteArray() {
    std::vector<char> zaBytes;
    zaBytes.reserve(localHeaderSize);

    appendArrayToVector(zaBytes, putWordLE(this->versionNeeded));
    appendArrayToVector(zaBytes, putWordLE(this->generalPurpose));
    appendArrayToVector(zaBytes, putWordLE(this->compressionMethod));
    appendArrayToVector(zaBytes, putWordLE(this->lastModTime));
    appendArrayToVector(zaBytes, putWordLE(this->lastModDate));
    appendArrayToVector(zaBytes, putDWordLE(this->crc32));
    appendArrayToVector(zaBytes, putDWordLE(this->compressedSize));
    appendArrayToVector(zaBytes, putDWordLE(this->uncompressedSize));
    appendArrayToVector(zaBytes, putWordLE(this->filenameLength));
    appendArrayToVector(zaBytes, putWordLE(this->extraLength));

    return zaBytes;
}

LocalHeader LocalHeader::readLocalHeader(ifstream& file, streampos at) {
    unsigned char buffer[localHeaderSize];

    file.clear();
    file.seekg(at, file.beg);
    file.read(reinterpret_cast<char*>(buffer), localHeaderSize);

    if (getDWordLE(buffer) != localHeaderMagic) {
        std::stringstream errMsg;
        errMsg << "Encountered a non-local header at "
                << std::hex << at;
        throw std::runtime_error(errMsg.str());
    }

    LocalHeader localHeader{
        .versionNeeded = getWordLE(buffer+4),
        .generalPurpose = getWordLE(buffer+6),
        .compressionMethod = getWordLE(buffer+8),
        .lastModTime = getWordLE(buffer+10),
        .lastModDate = getWordLE(buffer+12),
        .crc32 = getDWordLE(buffer+14),
        .compressedSize = getDWordLE(buffer+18),
        .uncompressedSize = getDWordLE(buffer+22),
        .filenameLength = getWordLE(buffer+26),
        .extraLength = getWordLE(buffer+28)
    };
    localHeader.data = file.tellg() + streamoff(localHeader.filenameLength + localHeader.extraLength);

    localHeader.filename.resize(localHeader.filenameLength);
    file.read(localHeader.filename.data(), localHeader.filenameLength);

    localHeader.extra.resize(localHeader.extraLength);
    file.read(localHeader.extra.data(), localHeader.extraLength);

    return localHeader;
}
