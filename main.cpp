#include <iostream>
#include <iomanip>
#include <fstream>

#include "zip/zip.h"

const size_t blockSize = 4096;
char buffer[blockSize];

int main(int argc, char const ** argv) {
    using namespace std;
    if (!(argc > 1)) {
        cout << "Argument is missing" << std::endl;
        exit(EXIT_FAILURE);
    }

    ZipFile zipfile{argv[1]};
    ifstream mainfile{"test.app",mainfile.ate | mainfile.binary};
    std::ofstream output{"output", output.binary};

    auto eomainfile = mainfile.tellg();
    auto eocdrPos   = zipfile.findEOCDR();

    if (eocdrPos != std::streampos(-1)) {
        cout << "EOCDR is at offset " << hex << eocdrPos << endl;
        auto eocdr = zipfile.readEOCDR(eocdrPos);

        cout << eocdr;

        // TODO: Build the central directory from the information in the eocdr
        auto cdr = zipfile.readCDRs(eocdr.startOfCDR, eocdr.currentDiskEntriesTotal);

        cout << "=== Central directory records ===" << endl;
        for (auto record : cdr) {
            cout << record << endl;
        }
        
        // TODO: Fetch all the local file headers (probably only the ones given by the cdrs we've collected)
        auto entries = zipfile.getZipEntries(cdr);

        cout << "=== Local headers ===" << endl;
        for (auto entry : entries) {
            cout << entry.localHeader << endl;
        }

        mainfile.seekg(0);
        output << mainfile.rdbuf();

        // TODO: Write out all the local file headers plus data payloads (the ones that we've collected)
        for (auto entry : entries) {
            auto header = entry.localHeader;

            output.write(reinterpret_cast<const char*>(&localHeaderMagic), sizeof(localHeaderMagic));
            output.write(header.getAsByteArray().data(), localHeaderSize-4);
            output.write(header.filename.data(), header.filenameLength);
            output.write(header.extra.data(), header.extraLength);
            
            zipfile.copyNTo(output, header.compressedSize, buffer, blockSize);
        }
        
        // TODO: Write archive extra data if there's any
        // TODO: Write CDR
        for (auto entry : entries) {
            auto cdr = entry.cdr;

            cdr.relOffset += eomainfile;
            
            cout << cdr << endl;

            output.write(reinterpret_cast<const char*>(&cdrMagic), sizeof(cdrMagic));
            output.write(cdr.getAsByteArray().data(), cdrSize-4);
            output.write(cdr.filename.data(), cdr.filenameLength);
            output.write(cdr.extra.data(), cdr.extraLength);
            output.write(cdr.comment.data(), cdr.commentLength);
        }
        // TODO: Write EOCDR

        cout << "Old EOCDR" << endl << eocdr << endl;
        eocdr.startOfCDR += eomainfile;
        cout << "New EOCDR" << endl << eocdr;

        output.write(reinterpret_cast<const char*>(&eocdrMagic), sizeof(eocdrMagic));
        output.write(eocdr.getAsByteArray().data(), eocdrSize-4);
        output.write(eocdr.comment.data(), eocdr.commentSize);
    } else {
        cout << "The given file is not a zipfile." << endl;
    }



    return 0;
}