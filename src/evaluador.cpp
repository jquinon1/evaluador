#include <iostream>
#include <cstdlib>
#include <stdio.h>
#include <string.h>
#include "initializer.h"
#include "registry.h"
#include "reporter.h"
#include "controller.h"
#include "helper.h"
#include "stop.h"

using namespace std;

int main(int argc, char* argv[]){
  if (argc <= 1) usage(argv[0]);
  char *command = argv[1];
  if (strcmp(command,"init") == 0) {
    initializer(argc,argv);
  } else if (strcmp(command,"reg") == 0) {
    registry(argc,argv);
  } else if (strcmp(command,"ctrl") == 0) {
    controller(argc,argv);
  } else if (strcmp(command,"rep") == 0) {
    cout << "Reporting" << endl;
    // call command method reporter(*argv)
    reporter(argc,argv);
  } else if (strcmp(command,"stop") == 0) {
    stop(argc,argv);
  } else {
    usage(argv[0]);
  }

  return EXIT_SUCCESS;
}
