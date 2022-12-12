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

    if (zipfile.eocdrPos != std::streampos(-1)) {
        mainfile.seekg(0);
        output << mainfile.rdbuf();

        auto entries = zipfile.entries;

        // Write out all the local file headers plus data payloads (the ones that we've collected)
        for (auto& entry : entries) {
                auto header = entry.localHeader;

                auto pos = output.tellp();
                entry.cdr.relOffset = pos;

                output.write(reinterpret_cast<const char*>(&localHeaderMagic), sizeof(localHeaderMagic));
                output.write(header.getAsByteArray().data(), localHeaderSize-4);
                output.write(header.filename.data(), header.filenameLength);
                output.write(header.extra.data(), header.extraLength);
                
                zipfile.copyNBytesTo(output, header.compressedSize, buffer, blockSize);
        }

        // TODO: Write archive extra data if there's any
        // Write updated CDR
        auto cdrPos = output.tellp();
        for (auto entry : entries) {
            auto cdr = entry.cdr;

            output.write(reinterpret_cast<const char*>(&cdrMagic), sizeof(cdrMagic));
            output.write(cdr.getAsByteArray().data(), cdrSize-4);
            output.write(cdr.filename.data(), cdr.filenameLength);
            output.write(cdr.extra.data(), cdr.extraLength);
            output.write(cdr.comment.data(), cdr.commentLength);
        }

        zipfile.eocdr.startOfCDR = cdrPos;

        // Write updated EOCDR
        output.write(reinterpret_cast<const char*>(&eocdrMagic), sizeof(eocdrMagic));
        output.write(zipfile.eocdr.getAsByteArray().data(), eocdrSize-4);
        output.write(zipfile.eocdr.comment.data(), zipfile.eocdr.commentSize);
    } else {
        cout << "The given file is not a zipfile." << endl;
    }

    return 0;
}