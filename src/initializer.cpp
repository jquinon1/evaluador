#include "initializer.h"
#include <iostream>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include "helper.h"

using namespace std;

// Define allowed options
const int number_parameters = 9;
const char *parameters[] = {"-i","-oe","-n","-b","-d","-ee","-s","-q","-ie"};

void initializer(int params_lenght,char *params[]) {
  // Check if the number of params has sense
  if(params_lenght % 2 != 0 ) usage(params[0]);
  // Check if params are valid
  for (int i = 2; i < params_lenght; i+=2) {
    if(!check_valid_param(number_parameters,parameters,params[i])) usage(params[0]);
  }
}
