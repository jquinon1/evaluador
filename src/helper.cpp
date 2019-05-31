#include "helper.h"
#include <cstdlib>
#include <iostream>
#include <string.h>

using namespace std;

void usage(char* progname) {
  cerr << "Usage: \n"
       << progname
       << "\n\tinit [-i <integer>] [-ie <integer>] [-oe <integer>]"
       << "\n\t     [-n <string>] [-b <integer>] [-d <integer>]"
       << "\n\t     [-s <integer>] [-q <integer>]"
       << "\n\treg [-n <string>] {{<filename>} ... | - }"
       << "\n\tctrl [-n <string>]"
       << "\n\t\tCTRL subcommands:"
       << "\n\t\t\tlist [ processing | waiting | reported | reactive | all ]"
       << "\n\t\t\tupdate { B | D | S } <integer>"
       << "\n\trep [-n <string>] { -i <integer> | -m <integer> }"
       << "\n\tstop [-n <string>]"
       << endl;
  exit(EXIT_FAILURE);
}

bool check_valid_param(const int num_params, const char *parameters[],char *argument){
  for (int i = 0; i < num_params; i++) {
    if(strcmp(argument,parameters[i]) == 0) return true;
  }
  return false;
}
