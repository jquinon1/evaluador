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
const char *parameters[] = {"-i","-ie","-oe","-n","-b","-d","-s","-q"};
// Control vars
char const *shared_mem_name = DEFAULT_SHM_NAME;
int custom_input = DEFAULT_INPUT;
int custom_input_lenght = DEFAULT_INPUT_LENGHT;
int custom_output = DEFAULT_OUTPUT;
int custom_reactive_blood = DEFAULT_REACTIVE_BLOOD;
int custom_reactive_detritos = DEFAULT_REACTIVE_DETRITOS;
int custom_reactive_skin = DEFAULT_REACTIVE_SKIN;
int custom_intern_queues = DEFAULT_INTERN_QUEUES;

// TO DO: Assign these values in a better way
void fill_custom_values(int params_lenght, char *parameters[]){
  int index;
  if((index = param_index(params_lenght,parameters,(char *)"-i")) != -1 ) custom_input = atoi(parameters[index+1]);
  if((index = param_index(params_lenght,parameters,(char *)"-ie")) != -1 ) custom_input_lenght = atoi(parameters[index+1]);
  if((index = param_index(params_lenght,parameters,(char *)"-oe")) != -1 ) custom_output = atoi(parameters[index+1]);
  if((index = param_index(params_lenght,parameters,(char *)"-n")) != -1 ) shared_mem_name = parameters[index+1];
  if((index = param_index(params_lenght,parameters,(char *)"-b")) != -1 ) custom_reactive_blood = atoi(parameters[index+1]);
  if((index = param_index(params_lenght,parameters,(char *)"-d")) != -1 ) custom_reactive_detritos = atoi(parameters[index+1]);
  if((index = param_index(params_lenght,parameters,(char *)"-s")) != -1 ) custom_reactive_skin = atoi(parameters[index+1]);
  if((index = param_index(params_lenght,parameters,(char *)"-q")) != -1 ) custom_intern_queues = atoi(parameters[index+1]);
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
  if((mapped = mmap(NULL,sizeof(struct Inbox),PROT_READ | PROT_WRITE, MAP_SHARED,sm,0)) == MAP_FAILED){
    cerr << "Error mapping shared memory: [" << errno << "] " << strerror(errno) << endl;
    exit(EXIT_FAILURE);
  }
  // HERE OR BEFORE, SET THE PARAMETERS YOU GET BY COMMAND LINE AND THEN CREATE RESOURCES
  // Creating input with the required inboxes
  struct Input *shInput = (struct Input *) mapped;
  shInput->in = 0;
  shInput->out = 0;
  shInput->current = 0;
  shInput->maximun = custom_input;
  for (int i = 0; i < custom_input; i++) {
    // Create the required semaphores for each inbox
    string empty_name = "inbox_" + to_string(i) + "_empty";
    string full_name = "inbox_" + to_string(i) + "_full";
    string mutex_name = "inbox_" + to_string(i) + "_mutex";
    shInput->Inboxes[i].in = 0;
    shInput->Inboxes[i].out = 0;
    shInput->Inboxes[i].current = 0;
    shInput->Inboxes[i].maximun = custom_input_lenght;
    shInput->Inboxes[i].empty = sem_open(empty_name.c_str(),O_CREAT | O_EXCL, 0660, custom_input_lenght);
    shInput->Inboxes[i].full = sem_open(full_name.c_str(),O_CREAT | O_EXCL, 0660, 0);
    shInput->Inboxes[i].mutex = sem_open(mutex_name.c_str(),O_CREAT | O_EXCL, 0660, 1);
  }
  // Create Struct for output with semaphores and element
}

void initializer(int params_lenght,char *params[]) {
  // Check if the number of params has sense
  if(params_lenght % 2 != 0 ) usage(params[0]);

  // Check if params are valid
  for (int i = 2; i < params_lenght; i+=2) {
    if(!check_valid_param(number_parameters,parameters,params[i])) usage(params[0]);
  }
  fill_custom_values(params_lenght,params);
  // Create the shm
  // create_resources(params_lenght,params);
  create_shm(shared_mem_name,params_lenght,params);
  // delete_shm(shared_mem_name);
  // for (int i = 2; i < params_lenght; i+=2) {
  //
  // }
}
