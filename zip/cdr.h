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
    WORD filenameLength() const { return filename.size(); }
    WORD extraLength() const { return extra.size(); }
    WORD commentLength() const { return comment.size(); }
    WORD diskNoStart;
    WORD internalAttr;
    DWORD externalAttr;
    DWORD relOffset;
    std::vector<char> filename;
    std::vector<char> extra;
    std::vector<char> comment;

    std::vector<char> getAsByteArray() const;
    
    size_t length() const;
};

std::ostream& operator<< (std::ostream& os, const CDR &record);

#endif // _CDR_H
