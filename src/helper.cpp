#include "helper.h"
#include <cstdlib>
#include <iostream>
#include <string>

using namespace std;

void usage(char* progname) {
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

bool check_valid_param(const int num_args, const char *parameters[],char *param){
  string argument = param;
  for (int i = 0; i < num_args; i++) {
    string parameter = parameters[i];
    if(argument == parameter) return true;
  }
  return false;
}
