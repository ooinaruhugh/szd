#if !defined(_LOCALHEADER_H)
#define _LOCALHEADER_H

#include "../util.h"
#include <iostream>

const DWORD localHeaderMagic = 0x04034b50;

const size_t localHeaderSize = 30;
class LocalHeader {
    // The position of the data in the zipfile. This is not part of the original header.
    std::streampos data;

    public:
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

    std::vector<char> getAsByteArray();
    static LocalHeader readLocalHeader(std::ifstream& file, std::streampos at);
};

std::ostream& operator<< (std::ostream& os, LocalHeader record);

#endif // _LOCALHEADER_H