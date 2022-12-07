#include <iostream>
#include <iomanip>
#include <fstream>

#include "zip/zip.h"

int main(int argc, char const ** argv) {
    using namespace std;
    if (!(argc > 1)) {
        cout << "Argument is missing" << std::endl;
        exit(EXIT_FAILURE);
    }

    ZipFile zipfile{argv[1]};
    ifstream mainfile{"test.app", std::ifstream::ate | std::ifstream::binary};
    std::ofstream output{"output", std::ofstream::binary};

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

        // mainfile.seekg(0);
        // output << mainfile.rdbuf();

        // TODO: Write out all the local file headers plus data payloads (the ones that we've collected)
        // vector<char> buffer;
        // for (auto header : localHeaders) {
        //     output.write(reinterpret_cast<const char*>(&localHeaderMagic), sizeof(localHeaderMagic));
        //     output.write(header.getAsByteArray().data(), localHeaderSize-4);
        //     output.write(header.filename.data(), header.filenameLength);
        //     output.write(header.extra.data(), header.extraLength);

        //     // auto compressedData= zipfile.copyDataAt(header.cdr->relOffset, header.compressedSize);
        //     // output.write(compressedData.data(), header.compressedSize);
        // }
        
        // TODO: Write archive extra data if there's any
        // TODO: Write CDR
        // TODO: Write EOCDR
    } else {
        cout << "The given file is not a zipfile." << endl;
    }



    return 0;
}