#ifndef _EOCDR_H
#define _EOCDR_H

#include <iostream>
#include <fstream>
#include "../util.h"

const DWORD eocdrMagic = 0x06054b50;
const DWORD eocdr64Magic = 0x06064b50;
const DWORD eocdr64LocatorMagic = 0x07064b50;

const size_t eocdrSize = 22;
using EOCDR = struct EOCDR {
    WORD numOfDisk;
    WORD numOfStartDisk;
    WORD currentDiskEntriesTotal;
    WORD entriesTotal;
    DWORD size;
    DWORD startOfCDR;
    WORD commentSize() const { return comment.size(); }
    std::vector<char> comment;

    EOCDR() {};
    EOCDR(std::ifstream& zipfile, std::streampos at);
    
    std::vector<char> getAsByteArray() const;
};

const size_t eocdr64Size = 46; 
using EOCDR64 = struct EOCDR64 {
    QWORD size;
    WORD versionMadeBy;
    WORD versionNeeded;
    DWORD numOfDisk;
    DWORD numOfStartDisk;
    QWORD currentDiskEntriesTotal;
    QWORD entriesTotal;
    QWORD startOfCDR;
    WORD commentSize() const { return data.size(); }
    std::vector<char> data;
};

const size_t eocdr64locatorSize = 16;
using EOCDR64Locator = struct EOCDR64Locator {
    DWORD numOfStartDisk;
    QWORD startOfEOCDR;
    DWORD numOfDisk;
};

std::ostream& operator<< (std::ostream& os, const EOCDR &eocdr);

#endif // _EOCDR_H
