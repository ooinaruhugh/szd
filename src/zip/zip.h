#ifndef _ZIP_H
#define _ZIP_H

#include <iostream>
#include <fstream>
#include <memory>
#include <string>
#include <vector>

#include "../util.h"
#include "eocdr.h"
#include "cdr.h"
#include "LocalHeader.h"

const DWORD archiveExtraMagic = 0x08064b50;
const DWORD digitalSignatureMagic = 0x05054b50;

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

using ZipArchive = struct ZipArchive {
    EOCDR eocdr;

    EOCDR64 eocdr64;
    EOCDR64Locator eocdr64Locator;

    std::vector<CDR> cdr;

    std::vector<LocalHeader> entries;

    // ArchiveDecryptionHeader crypt;
    ArchiveExtra extra;
};

class ZipFile {
    std::ifstream file;
    std::streampos eocdrPos;
public:
    ZipArchive zip;

    ZipFile(const char* filename);
    ZipFile(const std::string& filename);
    // ZipFile(const ZipFile& zipfile);

    /*
        Find the position of the end-of-central directory record at the end of the zip file.
    */
    std::streampos findEOCDR();
    std::streampos findMagic(DWORD magic);
    std::streampos findMagicFromEnd(DWORD magic);

    /// @brief Gets all central directory entries from the file. 
    /// @param beginAt 
    /// @param noOfRecords 
    /// @return 
    std::vector<CDR> readCDRs(std::streampos beginAt, WORD noOfRecords);

    /*
        Gets all zip entries with their associated headers and data from the file
        as specified by the central directory records in cdrs.
    */
    //std::vector<LocalHeader> getZipEntries(std::vector<CDR> cdrs);

    /*
        Copies n bytes from the input file to the output file using
        a buffer of size n_buffer.
    */
    //void copyNBytesTo(std::ofstream& outfile, size_t n, char* buffer, size_t n_buffer);

    /*
        Copies n bytes from the input file at the given offset to the output file using
        a buffer of size n_buffer.
    */
    //void copyNBytesAtTo(std::ofstream& outfile, std::streampos at, size_t n, char* buffer, size_t n_buffer);
};

// std::ostream& operator<< (std::ostream& os, ZipFile zipfile);
//std::ofstream& operator<< (std::ofstream& os, ZipFile zipfile);

#endif // _ZIP_H
