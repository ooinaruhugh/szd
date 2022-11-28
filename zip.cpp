#include "zip.h"

std::ostream& operator<< (std::ostream& os, EOCDR eocdr) {
    auto f(os.flags());

    os << "number of this disk:\t" 
       << std::dec << eocdr.numOfDisk << std::endl;
    os << "number of the disk with the\nstart of the central directory:\t"
       << eocdr.numOfStartDisk << std::endl;
    os << "total number of entries in the\ncentral directory on this disk:\t"
       << eocdr.currentEntriesTotal << std::endl;
    os << "total number of entries in\nthe central directory:\t"
       << eocdr.entriesTotal << std::endl;
    os << "size of the central directory:\t0x"
       << std::hex << std::setfill('0') << std::setw(2) << eocdr.size << std::endl;
    os << "offset of start of central\ndirectory with respect to\nthe starting disk number:\t0x"
       << eocdr.startOfCDR << std::endl;
    os << ".ZIP file comment length:\t0x"
       << eocdr.commentSize << std::endl;
    os << ".ZIP file comment:\t"
       << "<TODO: Here goes comment>" << std::endl;

    os.flags(f);
    return os;
}

std::streampos findEOCDR(std::ifstream &zipfile) {
    // TODO Handle random occurences of 0x06054B50 https://stackoverflow.com/questions/8593904/how-to-find-the-position-of-central-directory-in-a-zip-file)
    std::streampos central_directory;
    
    const int readBufferSize = 512;
    char searchBuffer[readBufferSize+1];

    char* needle = nullptr;

    central_directory = zipfile.tellg();

    do
    {
        if (central_directory > readBufferSize) 
            central_directory -= readBufferSize;
        else 
            central_directory = 0;
        
        zipfile.seekg(central_directory);

        zipfile.read(searchBuffer, readBufferSize);

        needle = (char*)memmem(searchBuffer, readBufferSize, &EOCDRMagic, 4);
    } while ((central_directory > 0) && needle == nullptr);
    
    return needle ? central_directory + (needle - searchBuffer) : std::streampos(-1);
}

EOCDR readEOCDR(std::ifstream &zipfile, std::streampos at) {
    char buffer[EOCDRSize];
    zipfile.clear();
    zipfile.seekg(at, std::ios_base::beg);

    std::cout << at << " " << zipfile.tellg() << std::endl;
    zipfile.read(buffer, EOCDRSize);

    EOCDR eocdr{
        .numOfDisk = readUInt16LE(buffer+4),
        .numOfStartDisk = readUInt16LE(buffer+6),
        .currentEntriesTotal = readUInt16LE(buffer+8),
        .entriesTotal = readUInt16LE(buffer+10),
        .size = readUInt32LE(buffer+12),
        .startOfCDR = readUInt32LE(buffer+16),
        .commentSize = readUInt16LE(buffer+20),
        .comment = NULL
    };

    for (unsigned c : buffer) {
        auto f{std::cout.flags()};

        std::cout << std::setfill('0') << std::setw(2) << std::hex << c << ' ';
        
        std::cout.flags(f);
    }

    std::cout << std::endl;

    return eocdr;
}