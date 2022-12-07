#include <cstring>
#include <iostream>
#include <fstream>
#include <vector>
#include <array>

#include "../util.h"

#ifndef _ZIP_H
#define _ZIP_H

const DWORD eocdrMagic = 0x06054b50;
const DWORD cdrMagic = 0x02014b50;
const DWORD localHeaderMagic = 0x04034b50;
const DWORD ddMagic = 0x08074b50;
const DWORD archiveExtraMagic = 0x08064b50;
const DWORD digitalSignatureMagic = 0x05054b50;
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
    WORD commentSize;
    std::vector<char> comment;
};

const size_t eocdr64Size = 22;
using EOCDR64 = struct EOCDR64 {
    QWORD size;
    WORD versionMadeBy;
    WORD versionNeeded;
    DWORD numOfDisk;
    DWORD numOfStartDisk;
    QWORD currentDiskEntriesTotal;
    QWORD entriesTotal;
    QWORD startOfCDR;
    WORD commentSize;
    char* data;
};

const size_t eocdr64locatorSize = 22;
using EOCDR64Locator = struct EOCDR64Locator {
    DWORD numOfStartDisk;
    QWORD startOfEOCDR;
    DWORD numOfDisk;
};

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
    WORD filenameLength;
    WORD extraLength;
    std::vector<char> filename;
    std::vector<char> extra;
    std::streampos data;

    std::vector<char> getAsByteArray();
};

const size_t dataDescriptorSize = 12;
using DataDescriptor = struct DataDescriptor {
    DWORD crc32;
    DWORD compressedSize;
    DWORD uncompressedSize;
};

// TODO: Archive decryption header

const size_t archiveExtraSize = 4;
using ArchiveExtra = struct ArchiveExtra {
    DWORD extraLength;
    char* data;
};

const size_t digitalSignatureSize = 2;
using DigitalSignature = struct DigitalSignature {
    WORD size;
    char* data;
};

using ZipEntry = struct ZipEntry {
    LocalHeader localHeader;
    CDR cdr;

    ZipEntry(LocalHeader localHeader, CDR cdr) { 
        this->localHeader = localHeader;
        this->cdr = cdr;
    }
};

class ZipFile {
    bool hasEocdrPos = false;
    std::streampos eocdrPos;

    bool hasEocdr = false;
    EOCDR eocdr;

    bool hasEntries = false;
    std::vector<ZipEntry> entries;
    
    std::ifstream file;

    public:
        ZipFile(const char *filename);

        std::streampos findEOCDR();
        EOCDR readEOCDR(std::streampos at);

        std::vector<CDR> readCDRs(std::streampos beginAt, WORD noOfRecords);
        LocalHeader readLocalHeader(std::streampos at);

        std::vector<ZipEntry> getZipEntries(std::vector<CDR> cdrs);

        std::vector<char> copyDataAt(std::streampos at, size_t n);
};

std::ostream& operator<< (std::ostream& os, EOCDR eocdr);
std::ostream& operator<< (std::ostream& os, CDR record);
std::ostream& operator<< (std::ostream& os, LocalHeader record);

#endif // _ZIP_H