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
const int init_number_parameters = 8;
const char *init_parameters[] = {"-i","-ie","-oe","-n","-b","-d","-s","-q"};
// TO DO: Assign these values in a better way
void fill_custom_values(int params_length, char *parameters[]){
  int index;
  if((index = param_index(params_length,parameters,(char *)"-i")) != -1 ) custom_input = atoi(parameters[index+1]);
  if((index = param_index(params_length,parameters,(char *)"-ie")) != -1 ) custom_input_length = atoi(parameters[index+1]);
  if((index = param_index(params_length,parameters,(char *)"-oe")) != -1 ) custom_output = atoi(parameters[index+1]);
  if((index = param_index(params_length,parameters,(char *)"-n")) != -1 ) shared_mem_name = parameters[index+1];
  if((index = param_index(params_length,parameters,(char *)"-b")) != -1 ) custom_reactive_blood = atoi(parameters[index+1]);
  if((index = param_index(params_length,parameters,(char *)"-d")) != -1 ) custom_reactive_detritos = atoi(parameters[index+1]);
  if((index = param_index(params_length,parameters,(char *)"-s")) != -1 ) custom_reactive_skin = atoi(parameters[index+1]);
  if((index = param_index(params_length,parameters,(char *)"-q")) != -1 ) custom_intern_queues = atoi(parameters[index+1]);
}

void create_shm(const char *shm_name){
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
  if(ftruncate(sm,sizeof(struct Resources)) != 0){
    cerr << "Error assigning initial size to shared memory: [" << errno << "] " << strerror(errno) << endl;
    exit(EXIT_FAILURE);
  }
  // Mapping for input
  void *mapped;

  if((mapped = mmap(NULL,sizeof(struct exam),PROT_READ | PROT_WRITE, MAP_SHARED,sm,0)) == MAP_FAILED){
    cerr << "Error mapping shared memory: [" << errno << "] " << strerror(errno) << endl;
    exit(EXIT_FAILURE);
  }

  // HERE OR BEFORE, SET THE PARAMETERS YOU GET BY COMMAND LINE AND THEN CREATE RESOURCES
  // Creating input with the required inboxes
  struct Resources *shResources = (struct Resources *) mapped;
  shResources->shInput.in = 0;
  shResources->shInput.out = 0;
  shResources->shInput.current = 0;
  shResources->shInput.maximun = custom_input;
  for (int i = 0; i < custom_input; i++) {
    // Create the required semaphores for each inbox
    string input_empty_name = string(shm_name) + "_inbox_" + to_string(i) + "_empty";
    string input_full_name = string(shm_name) + "_inbox_" + to_string(i) + "_full";
    string input_mutex_name = string(shm_name) + "_inbox_" + to_string(i) + "_mutex";
    shResources->shInput.Inboxes[i].in = 0;
    shResources->shInput.Inboxes[i].out = 0;
    shResources->shInput.Inboxes[i].current = 10;
    shResources->shInput.Inboxes[i].maximun = custom_input_length;
    if ((shResources->shInput.Inboxes[i].empty = sem_open(input_empty_name.c_str(),O_CREAT | O_EXCL, 0660, custom_input_length)) == SEM_FAILED){
      exit(EXIT_FAILURE);
    }
    if ((shResources->shInput.Inboxes[i].full = sem_open(input_full_name.c_str(),O_CREAT | O_EXCL, 0660, 0)) == SEM_FAILED){
      exit(EXIT_FAILURE);
    }
    if ((shResources->shInput.Inboxes[i].mutex = sem_open(input_mutex_name.c_str(),O_CREAT | O_EXCL, 0660, 1)) == SEM_FAILED){
      exit(EXIT_FAILURE);
    }
  }
  // Create the required semaphores for output
  string output_empty_name = string(shm_name) + "_output_empty";
  string output_full_name = string(shm_name) + "_output_full";
  string output_mutex_name = string(shm_name) + "_output_mutex";
  shResources->shOutput.in = 0;
  shResources->shOutput.out = 0;
  shResources->shOutput.current = 0;
  shResources->shOutput.maximun = custom_output;
  if ((shResources->shOutput.empty = sem_open(output_empty_name.c_str(),O_CREAT | O_EXCL, 0660, custom_output)) == SEM_FAILED){
    exit(EXIT_FAILURE);
  }
  if ((shResources->shOutput.full = sem_open(output_full_name.c_str(),O_CREAT | O_EXCL, 0660, 0)) == SEM_FAILED){
    exit(EXIT_FAILURE);
  }
  if ((shResources->shOutput.mutex = sem_open(output_mutex_name.c_str(),O_CREAT | O_EXCL, 0660, 1)) == SEM_FAILED){
    exit(EXIT_FAILURE);
  }
  // Storing needed vars
  shResources->reactive_blood = custom_reactive_blood;
  shResources->reactive_detritos = custom_reactive_detritos;
  shResources->reactive_skin = custom_reactive_skin;
  shResources->intern_queues = custom_intern_queues;

  close(sm);
}

void initializer(int params_length,char *params[]) {
  // Check if the number of params has sense
  if(params_length % 2 != 0 ) usage(params[0]);
  // Check if params are valid
  for (int i = 2; i < params_length; i+=2) {
    if(!check_valid_param(init_number_parameters,init_parameters,params[i])) usage(params[0]);
  }
  fill_custom_values(params_length,params);
  // Create the shm
  create_shm(shared_mem_name);
}
