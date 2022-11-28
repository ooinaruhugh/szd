#include <cinttypes>
#include <cstring>
#include <ios>
#include <iomanip>
#include <iostream>
#include <fstream>

#include "util.h"

#ifndef _ZIP_H
#define _ZIP_H

const uint32_t EOCDRMagic = 0x06054B50;

class Zipfile {};

const size_t EOCDRSize = 22;
using EOCDR = struct EOCDR {
    uint16_t numOfDisk;
    uint16_t numOfStartDisk;
    uint16_t currentEntriesTotal;
    uint16_t entriesTotal;
    uint32_t size;
    uint32_t startOfCDR;
    uint16_t commentSize;
    char* comment;
};

std::ostream& operator<< (std::ostream& os, EOCDR eocdr);

std::streampos findEOCDR(std::ifstream &zipfile);

EOCDR readEOCDR(std::ifstream &zipfile, std::streampos at);

#endif // _ZIP_H