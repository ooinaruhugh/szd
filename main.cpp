#include <iostream>
#include <iomanip>
#include <fstream>
#include <getopt.h>
#include <string>

#include <boost/program_options.hpp>
namespace po = boost::program_options;

#include "zip/zip.h"

using namespace std;

const size_t blockSize = 4096;
// Buffer for copying unchanged file contents
char buffer[blockSize];

void printUsage(const po::options_description &desc) {
    cout << "Usage: szd [options] infile zipfile" << endl << desc;
}

[[noreturn]] void processZipFile(string infile, string zipfile, string outfile) {
    using namespace std;

    ZipFile zipf{zipfile};
    ifstream donor{infile, donor.ate | donor.binary};
    ofstream outf{outfile, outf.binary};

    auto endOfDonor = donor.tellg();

    if (zipf.eocdrPos != std::streampos(-1)) {
        donor.seekg(0);
        outf << donor.rdbuf();

        outf << zipfile;
    } else {
        cout << "The given file is not a zipfile." << endl;
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}

int main(int argc, char const **argv) {

    po::options_description desc("Allowed options");
    desc.add_options()
        ("help", "print this help message")
        ("output,o", po::value<string>(), "name for output file, defaults to <infile>.new")
        ("infile", po::value< vector<string> >(), "file to append the zipfile to") 
        ("zipfile", po::value< vector<string> >(), "zipfile to be appended")
    ;

    po::positional_options_description p;
    p.add("infile", 1);
    p.add("zipfile", 1);


    po::variables_map vm;
    po::store(po::command_line_parser(argc, argv)
                            .options(desc)
                            .positional(p).run(), vm);
    po::notify(vm);

    if (vm.count("help")) {
        printUsage(desc);
        exit(EXIT_SUCCESS);
    }

    string infile;
    string zipfile;
    if (vm.count("infile") && vm.count("zipfile")) {
        infile  = vm["infile"].as< vector<string> >()[0];
        zipfile = vm["zipfile"].as< vector<string> >()[0];
    } else {
        cout << "Error: you have to specify an input zipfile and a input target file." << endl;
        printUsage(desc);

        exit(EXIT_FAILURE);
    }

    string outfile;
    if (vm.count("outfile")) {
        outfile = vm["outfile"].as< vector<string> >()[0];
    } else {
        outfile = infile + ".new";
    }

    processZipFile(infile, zipfile, outfile);

    exit(EXIT_SUCCESS);

}