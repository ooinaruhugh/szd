#ifndef _LOCALHEADER_H
#define _LOCALHEADER_H

#include "../util.h"
#include <iostream>

const DWORD localHeaderMagic = 0x04034b50;

const DWORD dataDescriptorMagic = 0x08074b50;
const size_t dataDescriptorSize = 12;
using DataDescriptor = struct DataDescriptor {
    DWORD crc32;
    DWORD compressedSize;
    DWORD uncompressedSize;
    
    std::vector<char> getAsByteArray() const;
};

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
    WORD filenameLength() const { return filename.size(); }
    WORD extraLength() const { return extra.size(); }
    std::vector<char> filename;
    std::vector<char> extra;
    // The position of the data in the zipfile. This is not part of the original header.
    std::streampos data;
    size_t indexOfCDR;
    DataDescriptor dataDescriptor;

    std::vector<char> getAsByteArray() const;
    bool hasDataDescriptor() const;
    static LocalHeader readLocalHeader(std::ifstream& file, std::streampos at);

    size_t length() const;
    
    DataDescriptor findDataDescriptor(std::ifstream& file) const;
};

std::ostream& operator<< (std::ostream& os, const LocalHeader &record);

#endif // _LOCALHEADER_H
