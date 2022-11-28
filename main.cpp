#include <iostream>
#include <fstream>

#include "zip.h"

int main(int argc, char const ** argv) {
    if (!(argc > 1)) {
        std::cout << "Argument is missing" << std::endl;
        exit(EXIT_FAILURE);
    }

    std::ifstream zipfile{argv[1], std::ifstream::ate | std::ifstream::binary};
    auto eocdr_pos = findEOCDR(zipfile);

    if (eocdr_pos != std::streampos(-1)) {
        std::cout << "EOCDR is at offset " << std::hex << eocdr_pos << std::endl;

        EOCDR eocdr = readEOCDR(zipfile, eocdr_pos);
        std::cout << eocdr;
    } else {
        std::cout << "The given file is not a zipfile." << std::endl;
    }

    return 0;
}