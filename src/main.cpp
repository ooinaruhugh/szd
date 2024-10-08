#include <iostream>
#include <iomanip>
#include <iterator>
#include <fstream>
#include <string>
#include <stdexcept>
#include <filesystem>

#include <boost/program_options.hpp>
namespace po = boost::program_options;

#include "zip/zip.h"
#include "zip/eocdr.h"

using namespace std;

const size_t blockSize = 4096;
// Buffer for copying unchanged file contents
char buffer[blockSize];

void printUsage(const po::options_description &desc) {
    cerr << "Usage: szd [options] infile zipfile" << endl << desc;
}

//void processZipFile(string infilePath, string zipfilePath, string outfilePath) {
//    ZipFile zipf{zipfilePath};
//
//    if (!filesystem::exists(infilePath)) {
//        stringstream errMsg;
//        errMsg << "Donor file " << infilePath << " does not exist.";
//        throw invalid_argument(errMsg.str());
//    }
//    ifstream donor{infilePath, donor.ate | donor.binary};
//    ofstream outf{outfilePath, outf.binary};
//
//    auto endOfDonor = donor.tellg();
//
//    donor.seekg(0);
//    copy(
//         istreambuf_iterator(donor),
//         istreambuf_iterator<char>(),
//         ostreambuf_iterator(outf)
//    );
//    //outf << donor.rdbuf();
//
//    //outf << zipf;
//}

int main(int argc, char const **argv) {
    try {
        string outfile{""};

        po::options_description generic("Allowed options");
        generic.add_options()
            ("help", "print this help message")
            ("output,o", po::value<string>(&outfile), "name for output file, defaults to <infile>.new")
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

        po::variables_map variables;
        po::store(po::command_line_parser(argc, argv)
                                .options(cmdline_options)
                                .positional(p).run(), variables);
        po::notify(variables);

        if (variables.count("help")) {
            printUsage(visible);
            exit(EXIT_SUCCESS);
        }
        
        string infile;
        string zipfile;
        if (variables.count("infile") && variables.count("zipfile")) {
            infile  = variables["infile"].as< vector<string> >()[0];
            zipfile = variables["zipfile"].as< vector<string> >()[0];
        } else {
            cerr << "Error: you have to specify an input zipfile and a input target file." << endl;
            printUsage(visible);

            exit(EXIT_FAILURE);
        }

        if (outfile.length() == 0) {
            outfile = infile + ".new";
        }

        //processZipFile(infile, zipfile, outfile);
        ZipFile zipf{zipfile};
        cout << zipf.zip.eocdr << endl;

        for (auto cdr = begin(zipf.zip.cdr), last = end(zipf.zip.cdr); cdr != last; ++cdr) {
          cout << *cdr << endl;
        }

        exit(EXIT_SUCCESS);
    } catch (const exception &e) {
        cerr << "error: " << e.what() << endl;
        exit(EXIT_FAILURE);
    }
}
