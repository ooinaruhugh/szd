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

void processZipFile(string infile, string zipfile, string outfile) {
    using namespace std;

    ZipFile zipf{zipfile};
    ifstream donor{infile, donor.ate | donor.binary};
    ofstream outf{outfile, outf.binary};

    auto endOfDonor = donor.tellg();

    donor.seekg(0);
    outf << donor.rdbuf();

    outf << zipf;
}

int main(int argc, char const **argv) {
    try {
        po::options_description generic("Allowed options");
        generic.add_options()
            ("help", "print this help message")
            ("output,o", po::value<string>(), "name for output file, defaults to <infile>.new")
        ;

        po::options_description hidden("Hidden options");
        hidden.add_options()
            ("infile", po::value< vector<string> >(), "file to append the zipfile to") 
            ("zipfile", po::value< vector<string> >(), "zipfile to be appended")
        ;

        po::positional_options_description p;
        p.add("infile", 1);
        p.add("zipfile", 1);

        po::options_description cmdline_options;
        cmdline_options.add(generic).add(hidden);

        po::options_description visible;
        visible.add(generic);

        po::variables_map vm;
        po::store(po::command_line_parser(argc, argv)
                                .options(cmdline_options)
                                .positional(p).run(), vm);
        po::notify(vm);

        if (vm.count("help")) {
            printUsage(visible);
            exit(EXIT_SUCCESS);
        }

        string infile;
        string zipfile;
        if (vm.count("infile") && vm.count("zipfile")) {
            infile  = vm["infile"].as< vector<string> >()[0];
            zipfile = vm["zipfile"].as< vector<string> >()[0];
        } else {
            cout << "Error: you have to specify an input zipfile and a input target file." << endl;
            printUsage(visible);

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
    } catch (exception e) {
        cout << e.what() << endl;
        exit(EXIT_FAILURE);
    }
}