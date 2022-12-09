#include <iostream>
#include <iomanip>
#include <fstream>
#include <getopt.h>

#include "zip/zip.h"

const size_t blockSize = 4096;
// Buffer for copying unchanged file contents
char buffer[blockSize];

int main(int argc, char const ** argv) {
    using namespace std;
    if (!(argc > 2)) {
        cout << "Argument is missing" << std::endl;
        exit(EXIT_FAILURE);
    }

    ZipFile zipfile{argv[2]};
    ifstream mainfile{argv[1], mainfile.ate | mainfile.binary};

    stringstream outfileName;
    outfileName << argv[1] << ".new";
    ofstream output{outfileName.str(), output.binary};

    auto eomainfile = mainfile.tellg();
    auto eocdrPos   = zipfile.findEOCDR();

    if (eocdrPos != std::streampos(-1)) {
        auto eocdr = zipfile.readEOCDR(eocdrPos);

        // Build the central directory from the information in the eocdr
        auto cdr = zipfile.readCDRs(eocdr.startOfCDR, eocdr.currentDiskEntriesTotal);
        
        // TODO: Fetch all the local file headers (probably only the ones given by the cdrs we've collected)
        auto entries = zipfile.getZipEntries(cdr);

        mainfile.seekg(0);
        output << mainfile.rdbuf();

        // Write out all the local file headers plus data payloads (the ones that we've collected)
        for (auto entry : entries) {
            auto header = entry.localHeader;

            output.write(reinterpret_cast<const char*>(&localHeaderMagic), sizeof(localHeaderMagic));
            output.write(header.getAsByteArray().data(), localHeaderSize-4);
            output.write(header.filename.data(), header.filenameLength);
            output.write(header.extra.data(), header.extraLength);
            
            zipfile.copyNBytesTo(output, header.compressedSize, buffer, blockSize);
        }
        
        // TODO: Write archive extra data if there's any
        // Write updated CDR
        for (auto entry : entries) {
            auto cdr = entry.cdr;

            cdr.relOffset += eomainfile;

            output.write(reinterpret_cast<const char*>(&cdrMagic), sizeof(cdrMagic));
            output.write(cdr.getAsByteArray().data(), cdrSize-4);
            output.write(cdr.filename.data(), cdr.filenameLength);
            output.write(cdr.extra.data(), cdr.extraLength);
            output.write(cdr.comment.data(), cdr.commentLength);
        }

        // Write updated EOCDR
        eocdr.startOfCDR += eomainfile;

        output.write(reinterpret_cast<const char*>(&eocdrMagic), sizeof(eocdrMagic));
        output.write(eocdr.getAsByteArray().data(), eocdrSize-4);
        output.write(eocdr.comment.data(), eocdr.commentSize);
    } else {
        cout << "The given file is not a zipfile." << endl;
    }

    return 0;
}