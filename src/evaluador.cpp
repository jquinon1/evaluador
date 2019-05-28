#include <iostream>
#include <cstdlib>
#include <stdio.h>
#include <string.h>

using namespace std;

static void usage(const char*);

int main(int argc, char* argv[]){
  if (argc <= 1) usage(argv[0]);
  char *command = argv[1];
  if (strcmp(command,"init") == 0) {
    cout << "Initializing" << endl;
  } else if (strcmp(command,"reg") == 0) {
    cout << "Registering" << endl;
  } else if (strcmp(command,"ctrl") == 0) {
    cout << "Controlling" << endl;
  } else if (strcmp(command,"rep") == 0) {
    cout << "Reporting" << endl;
  } else {
    usage(argv[0]);
  }
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
