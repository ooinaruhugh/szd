#ifndef _ZIP_H
#define _ZIP_H

#include <cstring>
#include <iostream>
#include <fstream>
#include <vector>
#include <array>

#include "../util.h"
#include "eocdr.h"
#include "cdr.h"
#include "LocalHeader.h"

const DWORD ddMagic = 0x08074b50;
const DWORD archiveExtraMagic = 0x08064b50;
const DWORD digitalSignatureMagic = 0x05054b50;

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
    // EncryptionHeader crypt;
    DataDescriptor dd;

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

    bool hasEocdr64 = false;
    bool hasEocdr64Loc = false;
    EOCDR64 eocdr64;
    EOCDR64Locator eocdr64Locator;

    bool hasCDR = false;
    std::vector<CDR> cdr;

    bool hasEntries = false;
    std::vector<ZipEntry> entries;

    // ArchiveDecryptionHeader crypt;
    bool hasExtra = false;
    ArchiveExtra extra;

    std::ifstream file;

    public:
        ZipFile(const char *filename);

        /*
            Find the position of the end-of-central directory record at the end of the zip file.
        */
        std::streampos findEOCDR();
        EOCDR readEOCDR(std::streampos at);

        /*
            Gets all central directory entries.
        */
        std::vector<CDR> readCDRs(std::streampos beginAt, WORD noOfRecords);

        /*
            Gets all zip entries with their associated headers and data.
        */
        std::vector<ZipEntry> getZipEntries(std::vector<CDR> cdrs);

        /* 
            Copies n bytes from the input file to the output file using
            a buffer of size n_buffer.
        */
        void copyNBytesTo(std::ofstream& outfile, size_t n, char* buffer, size_t n_buffer);
};

std::ostream& operator<< (std::ostream& os, EOCDR eocdr);
std::ostream& operator<< (std::ostream& os, CDR record);
std::ostream& operator<< (std::ostream& os, LocalHeader record);

#endif // _ZIP_H