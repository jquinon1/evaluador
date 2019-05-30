#include "initializer.h"
#include <iostream>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include "helper.h"

using namespace std;

void initializer(int params_lenght,char *params[]) {
  // cout << "No implemented yet" << endl;
  // cout << DEFAULT_INPUT << endl;
  // un metodo para crear todos los objectos indexados en memoria compartida
  for (int i = 0; i < params_lenght; i++) {
    std::cout << params[i] << endl;
  }
}
