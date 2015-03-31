// Copyright 2011, University of Freiburg,
// Chair of Algorithms and Data Structures.
// Author: Björn Buchhold <buchholb>

#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>
#include <string>
#include <vector>
#include <iomanip>
#include <iostream>

#include "engine/Server.h"
#include "util/ReadableNumberFact.h"

using std::string;
using std::vector;
using std::cout;
using std::endl;
using std::flush;
using std::cerr;

#define EMPH_ON  "\033[1m"
#define EMPH_OFF "\033[21m"

// Available options.
struct option options[] = {
  {"ontology-basename", required_argument, NULL, 'o'},
  {"port", required_argument, NULL, 'p'},
  {NULL, 0, NULL, 0}
};

void printUsage() {
  cout << "Usage: ./ServerMain -p <PORT> -o <ontology-basename>" << endl;
}

// Main function.
int main(int argc, char** argv) {
  cout << endl << EMPH_ON << "ServerMain, version " << __DATE__
       << " " << __TIME__<< EMPH_OFF << endl << endl;

  char* locale = setlocale(LC_CTYPE, "en_US.utf8");
  cout << "Set locale LC_CTYPE to: " << locale << endl;

  std::locale loc;
  ad_utility::ReadableNumberFacet facet(1);
  std::locale locWithNumberGrouping(loc, &facet);
  ad_utility::Log::imbue(locWithNumberGrouping);


  // Init variables that may or may not be
  // filled / set depending on the options.
  string ontologyBase = "";
  int port = -1;

  optind = 1;
  // Process command line arguments.
  while (true) {
    int c = getopt_long(argc, argv, "o:p:", options, NULL);
    if (c == -1) break;
    switch (c) {
      case 'o':
        ontologyBase = optarg;
        break;
      case 'p':
        port = atoi(optarg);
        break;
      default:
        cout << endl
             << "! ERROR in processing options (getopt returned '" << c
             << "' = 0x" << std::setbase(16) << static_cast<int> (c) << ")"
             << endl << endl;
        exit(1);
    }
  }

  if (ontologyBase.size() == 0 || port == -1) {
    printUsage();
    exit(1);
  }

  try {
    Server server(port);
    server.initialize(ontologyBase);
    server.run();
  } catch(const ad_semsearch::Exception& e) {
    LOG(ERROR) << e.getFullErrorMessage() << '\n';
    return 1;
  }
  return 0;
}