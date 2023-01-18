#include <iostream>
#include <iomanip>
#include "cdr.h"
using namespace std;

ostream& operator<<(ostream& os, CDR record) {
    auto f{os.flags()};

    os << "filename: " << string(record.filename.begin(), record.filename.end()) << "🔚" << endl;
    os << "offset: " << hex << record.relOffset << endl;

    // os << "version made by:\t" 
    //    << std::dec << record.versionMadeBy << std::endl;
    // os << "version needed to extract:\t" 
    //    << std::dec << record.versionNeeded << std::endl;
    os << "file name length:\t0x"
       << hex << setfill('0') << setw(2) << record.filenameLength() << endl;
    os << "extra field length:\t0x"
       << record.extraLength() << endl;
    os << "file comment length:\t0x"
       << record.commentLength() << endl;

    os.flags(f);
    return os;
}

vector<char> CDR::getAsByteArray() {
    vector<char> zaBytes;
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
    appendArrayToVector(zaBytes, putWordLE(this->filenameLength()));
    appendArrayToVector(zaBytes, putWordLE(this->extraLength()));
    appendArrayToVector(zaBytes, putWordLE(this->commentLength()));
    appendArrayToVector(zaBytes, putWordLE(this->diskNoStart));
    appendArrayToVector(zaBytes, putWordLE(this->internalAttr));
    appendArrayToVector(zaBytes, putDWordLE(this->externalAttr));
    appendArrayToVector(zaBytes, putDWordLE(this->relOffset));

    return zaBytes;
}
