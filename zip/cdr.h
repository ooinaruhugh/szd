#ifndef _CDR_H
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
    WORD filenameLength() { return filename.size(); }
    WORD extraLength() { return extra.size(); }
    WORD commentLength() { return comment.size(); }
    WORD diskNoStart;
    WORD internalAttr;
    DWORD externalAttr;
    DWORD relOffset;
    std::vector<char> filename;
    std::vector<char> extra;
    std::vector<char> comment;

    std::vector<char> getAsByteArray();
};

std::ostream& operator<< (std::ostream& os, CDR record);

#endif // _CDR_H
