#include <iostream>
#include <fstream>

#include "zip/zip.h"

int main(int argc, char const ** argv) {
    using namespace std;
    if (!(argc > 1)) {
        cout << "Argument is missing" << std::endl;
        exit(EXIT_FAILURE);
    }

    ifstream zipfile{argv[1], std::ifstream::ate | std::ifstream::binary};
    ifstream mainfile{"test.app", std::ifstream::ate | std::ifstream::binary};
    // std::ofstream output{"output", std::ofstream::binary};

    // auto inbuf      = zipfile.rdbuf();
    // auto outbuf     = mainfile.rdbuf();
    auto eomainfile = mainfile.tellg();

    auto eocdr_pos  = findEOCDR(zipfile);

    if (eocdr_pos != std::streampos(-1)) {
        cout << "EOCDR is at offset " << hex << eocdr_pos << endl;
        auto eocdr = readEOCDR(zipfile, eocdr_pos);

        cout << eocdr;

        // TODO: Build the central directory from the information in the eocdr
        auto cdr = readCDR(zipfile, eocdr.startOfCDR, eocdr.currentDiskEntriesTotal);

        cout << "=== Central directory records ===" << endl;
        for (auto record : cdr) {
            cout << record << endl;
        }
        
        // TODO: Fetch all the local file headers (probably only the ones given by the cdrs we've collected)
        // auto localHeaders = readLocalHeaders(zipfile, cdr);

        // cout << "=== Local headers ===" << endl;
        // for (auto record : localHeaders) {
        //     cout << string(record.filename.begin(), record.filename.end()) << endl;
        // }

        // TODO: Rewrite offsets
        // for (auto& record : cdr) {
        //     record.relOffset += eomainfile;
        // }

        // TODO: Write out all the local file headers plus data payloads (the ones that we've collected)
        // for (auto header : localHeaders) {
        //     mainfile.write((char*)localHeaderMagic,sizeof(localHeaderMagic));
        //     mainfile.write(header.getAsByteArray().data(), localHeaderSize);
        //     mainfile.write(header.filename.data(), header.filenameLength);
        //     mainfile.write(header.extra.data(), header.extraLength);

        //     zipfile.seekg(header.data, zipfile.beg);
        //     std::istreambuf_iterator<char> begin(zipfile);
        //     auto end = begin + header.compressedSize;
        // }
        
        // TODO: Write archive extra data if there's any
        // TODO: Write CDR
        // TODO: Write EOCDR
    } else {
        cout << "The given file is not a zipfile." << endl;
    }



    return 0;
}