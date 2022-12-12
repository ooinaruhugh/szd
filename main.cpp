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

        output << zipfile;
    } else {
        cout << "The given file is not a zipfile." << endl;
    }

    return 0;
}