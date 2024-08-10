#include <cstring>
#include <iostream>
#include <fstream>
#include <iomanip>

using namespace std;

#include "eocdr.h"

EOCDR::EOCDR(std::ifstream& zipfile, std::streampos at) {
    unsigned char buffer[eocdrSize];
    zipfile.clear();
    zipfile.seekg(at, zipfile.beg);
    zipfile.read(reinterpret_cast<char*>(buffer), eocdrSize);

    if (getDWordLE(buffer) != eocdrMagic) {
        throw invalid_argument("There is not an end of central directory record at the specified position.");
    }

    numOfDisk = getWordLE(buffer+4);
    numOfStartDisk = getWordLE(buffer+6);
    currentDiskEntriesTotal = getWordLE(buffer+8);
    entriesTotal = getWordLE(buffer+10);
    size = getDWordLE(buffer+12);
    startOfCDR = getDWordLE(buffer+16);

    auto commentSize = getWordLE(buffer+20);

    if (commentSize > 0) {
        comment.resize(commentSize);
        zipfile.read(comment.data(), commentSize);
    } 
}

ostream& operator<< (ostream& os, const EOCDR &eocdr) {
    auto f{os.flags()};

    os << left;
    os << setw(32) << "number of this disk:" << setw(0)
       << dec << eocdr.numOfDisk << endl;
    os << "number of the disk with the" << endl
       << setw(32) << "start of the central directory:" << setw(0)
       << eocdr.numOfStartDisk << endl;
    os << "total number of entries in the" << endl
       << setw(32) << "central directory on this disk:" << setw(0)
       << eocdr.currentDiskEntriesTotal << endl;
    os << "total number of entries in" << endl
       << setw(32) << "the central directory:" << setw(0)
       << eocdr.entriesTotal << endl;
    os << setw(32) << "size of the central directory:" << setw(0)
       << "0x" << hex << setfill('0') << setw(2) << eocdr.size << endl;
    os << setfill(' ');
    os << "offset of start of central" << endl
       << "directory with respect to" << endl
       << setw(32) << "the starting disk number:" << setw(0)
       << "0x" << eocdr.startOfCDR << endl;
    os << setw(32) << "ZIP file comment length:" << setw(0)
       << "0x" << eocdr.commentSize() << endl;
    if (eocdr.commentSize() > 0) {
        os << "ZIP file comment:"
        << string(eocdr.comment.begin(), eocdr.comment.end()) << endl;
    }

    os.flags(f);
    return os;
}

vector<char> EOCDR::getAsByteArray() const {
    vector<char> zaBytes;
    zaBytes.reserve(eocdrSize-4);

    appendArrayToVector(zaBytes, putWordLE(this->numOfDisk));
    appendArrayToVector(zaBytes, putWordLE(this->numOfStartDisk));
    appendArrayToVector(zaBytes, putWordLE(this->currentDiskEntriesTotal));
    appendArrayToVector(zaBytes, putWordLE(this->entriesTotal));
    appendArrayToVector(zaBytes, putDWordLE(this->size));
    appendArrayToVector(zaBytes, putDWordLE(this->startOfCDR));
    appendArrayToVector(zaBytes, putWordLE(this->commentSize()));

    return zaBytes;
}
