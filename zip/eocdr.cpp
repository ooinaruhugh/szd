#include <cstring>
#include <iostream>
#include <fstream>
#include <iomanip>

using namespace std;

#include "eocdr.h"

ostream& operator<< (ostream& os, EOCDR eocdr) {
    auto f{os.flags()};

    os << "number of this disk:\t" 
       << dec << eocdr.numOfDisk << endl;
    os << "number of the disk with the\nstart of the central directory:\t"
       << eocdr.numOfStartDisk << endl;
    os << "total number of entries in the\ncentral directory on this disk:\t"
       << eocdr.currentDiskEntriesTotal << endl;
    os << "total number of entries in\nthe central directory:\t"
       << eocdr.entriesTotal << endl;
    os << "size of the central directory:\t0x"
       << hex << std::setfill('0') << setw(2) << eocdr.size << endl;
    os << "offset of start of central\ndirectory with respect to\nthe starting disk number:\t0x"
       << eocdr.startOfCDR << endl;
    os << ".ZIP file comment length:\t0x"
       << eocdr.commentSize() << endl;
    if (eocdr.commentSize() > 0) {
        os << ".ZIP file comment:\t"
        << string(eocdr.comment.begin(), eocdr.comment.end()) << endl;
    }

    os.flags(f);
    return os;
}

vector<char> EOCDR::getAsByteArray() {
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