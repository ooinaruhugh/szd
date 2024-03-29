#include "LocalHeader.h"
#include <iomanip>
#include <fstream>
#include <sstream>
using namespace std;

ostream& operator<< (std::ostream& os, const LocalHeader &record) {
    auto f{ os.flags() };

    os << "filename: " << string(record.filename.begin(), record.filename.end()) << "🔚" << endl;
    os << "compressed (actual, right-now) size: " << hex << record.compressedSize << endl;

    os << "file name length:\t0x"
        << hex << setfill('0') << std::setw(2) << record.filenameLength() << endl;
    os << "extra field length:\t0x"
        << record.extraLength() << endl;

    os.flags(f);
    return os;
}

vector<char> LocalHeader::getAsByteArray() const {
    vector<char> zaBytes;
    zaBytes.reserve(localHeaderSize);

    appendArrayToVector(zaBytes, putWordLE(this->versionNeeded));
    appendArrayToVector(zaBytes, putWordLE(this->generalPurpose));
    appendArrayToVector(zaBytes, putWordLE(this->compressionMethod));
    appendArrayToVector(zaBytes, putWordLE(this->lastModTime));
    appendArrayToVector(zaBytes, putWordLE(this->lastModDate));
    appendArrayToVector(zaBytes, putDWordLE(this->crc32));
    appendArrayToVector(zaBytes, putDWordLE(this->compressedSize));
    appendArrayToVector(zaBytes, putDWordLE(this->uncompressedSize));
    appendArrayToVector(zaBytes, putWordLE(this->filenameLength()));
    appendArrayToVector(zaBytes, putWordLE(this->extraLength()));

    return zaBytes;
}

bool LocalHeader::hasDataDescriptor() const {
    return nthBitIsSet(generalPurpose, 3);
}

LocalHeader LocalHeader::readLocalHeader(ifstream& file, streampos at) {
    unsigned char buffer[localHeaderSize];

    file.clear();
    file.seekg(at, file.beg);
    file.read(reinterpret_cast<char*>(buffer), localHeaderSize);

    if (getDWordLE(buffer) != localHeaderMagic) {
        stringstream errMsg;
        errMsg << "Encountered a non-local header at " << std::hex << at;
        throw runtime_error(errMsg.str());
    }

    LocalHeader localHeader{
        .versionNeeded = getWordLE(buffer + 4),
        .generalPurpose = getWordLE(buffer + 6),
        .compressionMethod = getWordLE(buffer + 8),
        .lastModTime = getWordLE(buffer + 10),
        .lastModDate = getWordLE(buffer + 12),
        .crc32 = getDWordLE(buffer + 14),
        .compressedSize = getDWordLE(buffer + 18),
        .uncompressedSize = getDWordLE(buffer + 22)
    };
    
    if (localHeader.hasDataDescriptor()) {
        localHeader.dataDescriptor = localHeader.findDataDescriptor(file);
    }

    auto filenameLength = getWordLE(buffer + 26);
    auto extraLength = getWordLE(buffer + 28);

    localHeader.data = file.tellg() + streamoff(filenameLength + extraLength);

    localHeader.filename.resize(filenameLength);
    file.read(localHeader.filename.data(), filenameLength);

    localHeader.extra.resize(extraLength);
    file.read(localHeader.extra.data(), extraLength);

    return localHeader;
}

size_t LocalHeader::length() const {
    return localHeaderSize + filenameLength() + extraLength() + compressedSize;
}

DataDescriptor LocalHeader::findDataDescriptor(ifstream& file) const {
    throw runtime_error("Data descriptor is not implemented yet.");
    
    // TODO: Variant 1
    // - Look for magic number (up to uncompressed size)
    // - if DD magic is found, check whether distance is exactly compressed size of DD
    // - otherwise, look for next
    
    // TODO: Variant 2
    // - Do sliding window, just take the one where uncompressed size and maybe crc32 lines up
    
    // TODO: Variant 3
    // - Look til next local header, then look backwards from there
    
    return {};
}
