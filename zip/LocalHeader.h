#if !defined(_LOCALHEADER_H)
#define _LOCALHEADER_H

#include "../util.h"
#include <iostream>

const DWORD localHeaderMagic = 0x04034b50;

const size_t localHeaderSize = 30;
using LocalHeader = struct LocalHeader {
    WORD versionNeeded;
    WORD generalPurpose;
    WORD compressionMethod;
    WORD lastModTime;
    WORD lastModDate;
    DWORD crc32;
    DWORD compressedSize;
    DWORD uncompressedSize;
    WORD filenameLength;
    WORD extraLength;
    std::vector<char> filename;
    std::vector<char> extra;
    std::streampos data;

    std::vector<char> getAsByteArray();
    static LocalHeader readLocalHeader(std::ifstream& file, std::streampos at);
};

#endif // _LOCALHEADER_H