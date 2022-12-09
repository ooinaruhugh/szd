#include <iostream>
#include <iomanip>
#include "cdr.h"

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

std::vector<char> CDR::getAsByteArray() {
    std::vector<char> zaBytes;
    zaBytes.reserve(cdrSize-4);

    appendArrayToVector(zaBytes, putWordLE(this->versionMadeBy));
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
    appendArrayToVector(zaBytes, putWordLE(this->commentLength));
    appendArrayToVector(zaBytes, putWordLE(this->diskNoStart));
    appendArrayToVector(zaBytes, putWordLE(this->internalAttr));
    appendArrayToVector(zaBytes, putDWordLE(this->externalAttr));
    appendArrayToVector(zaBytes, putDWordLE(this->relOffset));

    return zaBytes;
}
