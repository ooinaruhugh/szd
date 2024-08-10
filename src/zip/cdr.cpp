#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include "cdr.h"
using namespace std;

CDR::CDR(ifstream &zipfile, streampos at) {
    unsigned char buffer[cdrSize];

    zipfile.clear();
    zipfile.seekg(at, zipfile.beg);
    zipfile.read(reinterpret_cast<char*>(buffer), cdrSize);

    if (getDWordLE(buffer) != cdrMagic) {
        stringstream errMsg;
        errMsg << "Encountered a non-central directory record at "
               << hex << at;
        throw runtime_error(errMsg.str());
    }

    versionMadeBy = getWordLE(buffer+4);
    versionNeeded = getWordLE(buffer+6);
    generalPurpose = getWordLE(buffer+8);
    compressionMethod = getWordLE(buffer+10);
    lastModTime = getWordLE(buffer+12);
    lastModDate = getWordLE(buffer+14);
    crc32 = getDWordLE(buffer+16);
    compressedSize = getDWordLE(buffer+20);
    uncompressedSize = getDWordLE(buffer+24);
    diskNoStart = getWordLE(buffer+34);
    internalAttr = getWordLE(buffer+36);
    externalAttr = getDWordLE(buffer+38);
    relOffset = getDWordLE(buffer+42);

    auto filenameLength = getWordLE(buffer+28);
    auto extraLength = getWordLE(buffer+30);
    auto commentLength = getWordLE(buffer+32);

    filename.resize(filenameLength);
    zipfile.read(filename.data(), filenameLength);

    extra.resize(extraLength);
    zipfile.read(extra.data(), extraLength);

    comment.resize(commentLength);
    zipfile.read(comment.data(), commentLength);
}

ostream& operator<<(ostream& os, const CDR &record) {
    const unsigned short labelWidth = 21;
    auto f{os.flags()};

    os << left;
    os << setw(labelWidth) << "filename:" << setw(0)
       << string(record.filename.begin(), record.filename.end()) << "🔚" << endl;
    os << setw(labelWidth) << "offset:" << setw(0) 
       << "0x" << hex << record.relOffset << endl;

    // os << "version made by:\t" 
    //    << std::dec << record.versionMadeBy << std::endl;
    // os << "version needed to extract:\t" 
    //    << std::dec << record.versionNeeded << std::endl;
    os << setw(labelWidth) << "file name length:" << setw(0)
       << "0x" << hex << record.filenameLength() << endl;
    os << setw(labelWidth) << "extra field length:" << setw(0)
       << "0x" << record.extraLength() << endl;
    os << setw(labelWidth) << "file comment length:" << setw(0)
       << "0x" << record.commentLength() << endl;

    os.flags(f);
    return os;
}

vector<char> CDR::getAsByteArray() const {
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

size_t CDR::length() const
{
    return cdrSize + filenameLength() + extraLength() + commentLength();
}
