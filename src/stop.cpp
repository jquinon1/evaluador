#include "stop.h"
#include <iostream>
#include "helper.h"
#include <cstring>
#include <string.h>
#include <unistd.h>
#include "initializer.h"
#include <fcntl.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <sys/mman.h>

using namespace std;
// Define allowed options
const int stop_number_parameters = 1;
const char *stop_parameters[] = {"-n"};

void delete_sh_resources(const char* sh_mem_name){
  int inboxes = 0;
  // Removing semaphores for input
  int sm = shm_open(sh_mem_name, O_RDWR, 0660);
  void *mapped;
  if((mapped = mmap(NULL,sizeof(struct Inbox),PROT_READ | PROT_WRITE, MAP_SHARED,sm,0)) == MAP_FAILED){
    cerr << "Error mapping shared memory: [" << errno << "] " << strerror(errno) << endl;
    exit(EXIT_FAILURE);
  }
  struct Input *shInput = (struct Input *) mapped;
  inboxes = shInput->maximun;
  close(sm);
  for (int i = 0; i < inboxes; i++) {
    string empty_name = string(sh_mem_name) + "_inbox_" + to_string(i) + "_empty";
    string full_name = string(sh_mem_name) + "_inbox_" + to_string(i) + "_full";
    string mutex_name = string(sh_mem_name) + "_inbox_" + to_string(i) + "_mutex";
    sem_unlink(empty_name.c_str());
    sem_unlink(full_name.c_str());
    sem_unlink(mutex_name.c_str());
  }
  // HERE JUST REMOVE THE OUTPUT AND ITS SEMAPHORES
  // Removing Shared Memory
  string temp_name = "/" + string(sh_mem_name);
  shm_unlink(temp_name.c_str());
}

void stop(int params_lenght,char *params[]) {
  // Check if the number of params has sense
  if(params_lenght % 2 != 0 ) usage(params[0]);

  // Check if params are valid
  for (int i = 2; i < params_lenght; i+=2) {
    if(!check_valid_param(stop_number_parameters,stop_parameters,params[i])) usage(params[0]);
  }
  int index;
  if((index = param_index(params_lenght,params,(char *)"-n")) != -1 ) shared_mem_name = params[index+1];
  delete_sh_resources(shared_mem_name);
}
