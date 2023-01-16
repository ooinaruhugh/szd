#ifndef _ZIP_H
#define _ZIP_H

#include <cstring>
#include <iostream>
#include <fstream>
#include <vector>
#include <array>
#include <memory>

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

    CDR* const cdr;

    ZipEntry(LocalHeader localHeader, CDR &cdr) : cdr(&cdr) { 
        this->localHeader = localHeader;
    }

    ZipEntry(LocalHeader localHeader, CDR &cdr, DataDescriptor dd) : ZipEntry(localHeader, cdr) { 
        this->dd = dd;
    }
};

class ZipFile {
    std::shared_ptr<std::ifstream> file;
public:
    std::streampos eocdrPos;
    EOCDR eocdr;

    EOCDR64 eocdr64;
    EOCDR64Locator eocdr64Locator;

    std::vector<CDR> cdr;

    std::vector<ZipEntry> entries;

    // ArchiveDecryptionHeader crypt;
    ArchiveExtra extra;

    ZipFile(const char *filename);
    ZipFile(const std::string &filename);
    // ZipFile(const ZipFile& zipfile);

    /*
        Find the position of the end-of-central directory record at the end of the zip file.
    */
    std::streampos findEOCDR();
    std::streampos findMagic(DWORD magic);
    std::streampos findMagicFromEnd(DWORD magic);

    
    /// @brief  Reads the end-of-central directory record at the specified position, 
    ///         if there is one.
    /// @param at 
    /// @return 
    EOCDR readEOCDR(std::streampos at);

    /// @brief Gets all central directory entries from the file. 
    ///        Replaces the currently retained central directory entries.
    /// @param beginAt 
    /// @param noOfRecords 
    /// @return 
    std::vector<CDR> readCDRs(std::streampos beginAt, WORD noOfRecords);

    /*
        Gets all zip entries with their associated headers and data from the file
        as specified by the central directory records in cdrs.
    */
    std::vector<ZipEntry> getZipEntries(std::vector<CDR> cdrs);

    /* 
        Copies n bytes from the input file to the output file using
        a buffer of size n_buffer.
    */
    void copyNBytesTo(std::ofstream& outfile, size_t n, char* buffer, size_t n_buffer);

    /* 
        Copies n bytes from the input file at the given offset to the output file using
        a buffer of size n_buffer.
    */
    void copyNBytesAtTo(std::ofstream& outfile, std::streampos at, size_t n, char* buffer, size_t n_buffer);
};

// std::ostream& operator<< (std::ostream& os, ZipFile zipfile);
std::ofstream& operator<< (std::ofstream& os, ZipFile zipfile);

#endif // _ZIP_H