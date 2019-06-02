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
  int outputs = 0;
  int blood = 0;
  int detritos = 0;
  int skin = 0;
  int queues = 0;
  // Removing semaphores for input
  int sm = shm_open(sh_mem_name, O_RDWR, 0660);
  if( sm < 0){
    cerr << "Error opening shared memory: [" << errno << "] "<< strerror(errno) <<endl;
    exit(EXIT_FAILURE);
  }
  void *mapped;
  if((mapped = mmap(NULL,sizeof(struct exam),PROT_READ | PROT_WRITE, MAP_SHARED,sm,0)) == MAP_FAILED){
    cerr << "Error mapping shared memory: [" << errno << "] " << strerror(errno) << endl;
    exit(EXIT_FAILURE);
  }
  struct Resources *shResources = (struct Resources *) mapped;
  blood = shResources->reactive_blood;
  detritos = shResources->reactive_detritos;
  skin = shResources->reactive_skin;
  queues = shResources->intern_queues;
  outputs = shResources->shOutput.maximun;
  inboxes = shResources->shInput.maximun;
  std::cout << inboxes << '\n';
  std::cout << outputs << '\n';
  std::cout << blood << '\n';
  std::cout << detritos << '\n';
  std::cout << skin << '\n';
  std::cout << queues << '\n';
  close(sm);
  for (int i = 0; i < inboxes; i++) {
    string input_empty_name = string(sh_mem_name) + "_inbox_" + to_string(i) + "_empty";
    string input_full_name = string(sh_mem_name) + "_inbox_" + to_string(i) + "_full";
    string input_mutex_name = string(sh_mem_name) + "_inbox_" + to_string(i) + "_mutex";
    if (sem_unlink(input_empty_name.c_str()) != 0) {
      cerr << "Error delete semaphore: [" << errno << "] " << strerror(errno) << endl;
      exit(EXIT_FAILURE);
    }
    if (sem_unlink(input_full_name.c_str()) != 0) {
      cerr << "Error delete semaphore: [" << errno << "] " << strerror(errno) << endl;
      exit(EXIT_FAILURE);
    }
    if (sem_unlink(input_mutex_name.c_str()) != 0) {
      cerr << "Error delete semaphore: [" << errno << "] " << strerror(errno) << endl;
      exit(EXIT_FAILURE);
    }
  }
  // Removing semaphores for output
  string output_empty_name = string(sh_mem_name) + "_output_empty";
  string output_full_name = string(sh_mem_name) + "_output_full";
  string output_mutex_name = string(sh_mem_name) + "_output_mutex";
  sem_unlink(output_empty_name.c_str());
  sem_unlink(output_full_name.c_str());
  sem_unlink(output_mutex_name.c_str());
  // Removing Shared Memory
  string temp_name = "/" + string(sh_mem_name);
  shm_unlink(temp_name.c_str());
}

void stop(int params_length,char *params[]) {
  // Check if the number of params has sense
  if(params_length % 2 != 0 ) usage(params[0]);

  // Check if params are valid
  for (int i = 2; i < params_length; i+=2) {
    if(!check_valid_param(stop_number_parameters,stop_parameters,params[i])) usage(params[0]);
  }
  int index;
  if((index = param_index(params_length,params,(char *)"-n")) != -1 ) shared_mem_name = params[index+1];
  delete_sh_resources(shared_mem_name);
}
