#include "initializer.h"
#include <iostream>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "helper.h"
#include <cstring>
#include <errno.h>

using namespace std;

// Define allowed options
const int number_parameters = 8;
const char *parameters[] = {"-i","-oe","-n","-b","-d","-ee","-s","-q","-ie"};

void delete_shm(const char *shm_name){
  shm_unlink(shm_name);
}

void create_shm(const char *shm_name,int num_params,char *parameters[]){
  // Create shm, fails if already exist
  int sm = shm_open(shm_name, O_RDWR | O_CREAT | O_EXCL, 0660);
  // Create shm, truncate an existent if exist
  // int sm = shm_open(shm_name, O_RDWR | O_CREAT | O_TRUNC, 0660);
  if( sm < 0){
    cerr << "Error creating shared memory: [" << errno << "] "<< strerror(errno) <<endl;
    exit(EXIT_FAILURE);
  }
  // Assign initial space to shared mem, for now 4098
  // TO DO: Assign the right size depending on the objects that need to be created
  if(ftruncate(sm,4098) != 0){
    cerr << "Error assigning initial size to shared memory: [" << errno << "] " << strerror(errno) << endl;
    exit(EXIT_FAILURE);
  }
  // Mapping
  void *mapped;
  if((mapped = mmap(NULL,sizeof(struct Input),PROT_READ | PROT_WRITE, MAP_SHARED,sm,0)) == MAP_FAILED){
    cerr << "Error mapping shared memory: [" << errno << "] " << strerror(errno) << endl;
    exit(EXIT_FAILURE);
  }
  // HERE OR BEFORE, SET THE PARAMETERS YOU GET BY COMMAND LINE AND THEN CREATE RESOURCES
  struct Input *shInput = (struct Input *) mapped;
  shInput->in = 0;
  shInput->out = 0;
  shInput->current = 0;
  shInput->in = 0;

}

void initializer(int params_lenght,char *params[]) {
  // Check if the number of params has sense
  if(params_lenght % 2 != 0 ) usage(params[0]);
  // Check if params are valid
  for (int i = 2; i < params_lenght; i+=2) {
    if(!check_valid_param(number_parameters,parameters,params[i])) usage(params[0]);
  }
  // Create the shm
  char const *shared_mem_name = DEFAULT_SHM_NAME;
  for (int i = 0; i < params_lenght; i+=2) {
    if(strcmp("-n",params[i]) == 0) shared_mem_name = params[i+1];
  }
  // create_resources(params_lenght,params);
  create_shm(shared_mem_name,params_lenght,params);
  // delete_shm(shared_mem_name);
  // for (int i = 2; i < params_lenght; i+=2) {
  //
  // }
}
