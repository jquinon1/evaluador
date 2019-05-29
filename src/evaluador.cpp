#include <iostream>
#include <cstdlib>
#include <stdio.h>
#include <string.h>
#include "initializer.h"
#include "registry.h"
#include "reporter.h"
#include "controller.h"

using namespace std;

static void usage(const char*);

int main(int argc, char* argv[]){
  if (argc <= 1) usage(argv[0]);
  char *command = argv[1];
  if (strcmp(command,"init") == 0) {
    cout << "Initializing" << endl;
    // call command method initialize(*argv)
    initializer(*argv);
  } else if (strcmp(command,"reg") == 0) {
    cout << "registrying" << endl;
    // call command method registry(*argv)
    registry(*argv);
  } else if (strcmp(command,"ctrl") == 0) {
    cout << "Controlling" << endl;
    // call command method controller(*argv)
    controller(*argv);
  } else if (strcmp(command,"rep") == 0) {
    cout << "Reporting" << endl;
    // call command method reporter(*argv)
    reporter(*argv);
  } else {
    usage(argv[0]);
  }

  return EXIT_SUCCESS;
}

static void usage(const char* progname) {
  cerr << "Usage: \n"
       << progname
       << "\n\tinit [-i <integer>] [-ie <integer>] [-oe <integer>]"
       << "\n\t     [-n <string>] [-b <integer>] [-d <integer>]"
       << "\n\t     [-ee <integer>] [-s <integer>] [-q <integer>]"
       << "\n\treg [-n <string>] {{<filename>} ... | - }"
       << "\n\tctrl [-s <string>]"
       << "\n\t\tCTRL subcommands:"
       << "\n\t\t\tlist [ processing | waiting | reported | reactive | all ]"
       << "\n\t\t\tupdate { B | D | S } <integer>"
       << "\n\trep [-s <string>] { -i <integer> | -m <integer> }"
       << endl;
  exit(EXIT_FAILURE);
}
