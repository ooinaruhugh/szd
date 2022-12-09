#if !defined(_CDR_H)
#define _CDR_H

#include "../util.h"

const DWORD cdrMagic = 0x02014b50;

const size_t cdrSize = 46;
using CDR = struct CDR {
    WORD versionMadeBy;
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
    WORD commentLength;
    WORD diskNoStart;
    WORD internalAttr;
    DWORD externalAttr;
    DWORD relOffset;
    std::vector<char> filename;
    std::vector<char> extra;
    std::vector<char> comment;

    std::vector<char> getAsByteArray();
};


#endif // _CDR_H
