#include "reporter.h"
#include <iostream>
#include "helper.h"
#include "string.h"
#include <time.h>
#include <semaphore.h>

using namespace std;

const int rep_number_parameters = 3;
const char *rep_parameters[] = {"-n","-i","-m"};

void time_out(const char *shm_name, int indicator){
  std::cout << "not implemented yet" << '\n';
}

void times(const char *shm_name, int indicator){
  string temp_name = "/" + string(shm_name);
  int sm = shm_open(temp_name.c_str(), O_RDWR, 0660);
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
  sem_t *output_mutex, *output_full, *output_empty;
  string output_empty_name = string(shm_name) + "_output_empty";
  string output_full_name = string(shm_name) + "_output_full";
  string output_mutex_name = string(shm_name) + "_output_mutex";
  output_empty = sem_open(output_empty_name.c_str(), 0);
  output_full = sem_open(output_full_name.c_str(), 0);
  output_mutex = sem_open(output_mutex_name.c_str(), 0);
  for (int i = 0; i < indicator; i++) {
    sem_wait(output_full);
    sem_wait(output_mutex);
    int output_position_out = shResources->shOutput.out;
    exam result = shResources->shOutput.exams_ready[output_position_out];
    shResources->shOutput.exams_ready[output_position_out].reported = false;
    shResources->shOutput.exams_ready[output_position_out].waiting = false;
    shResources->shOutput.exams_ready[output_position_out].processing = false;
    shResources->shOutput.out = (output_position_out + 1) % shResources->shOutput.maximun;
    shResources->shOutput.current--;
    cout << "["
    << result.id << " "
    << result.inbox << " "
    << result.sample << " "
    << "NOTHING YET]"
    << endl;
    sem_post(output_mutex);
    sem_post(output_empty);
  }
  close(sm);
}

void reporter(int params_length,char *params[]) {
  // Check if the number of params has sense
  if(params_length % 2 != 0 ) usage(params[0]);
  if(params_length < 3) usage(params[0]);
  // Check if params are valid
  for (int i = 2; i < params_length; i+=2) {
    if(!check_valid_param(rep_number_parameters,rep_parameters,params[i])) usage(params[0]);
  }
  int index;
  if((index = param_index(params_length,params,(char *)"-n")) != -1 ) shared_mem_name = params[index+1];
  char *option = (index == -1) ? params[2] : params[index+2];
  int indicator = (index == -1) ? atoi(params[3]) : atoi(params[index+3]);
  std::cout << option << indicator << '\n';
  if(strcmp(option,"-i") == 0) time_out(shared_mem_name,indicator);
  if(strcmp(option,"-m") == 0) times(shared_mem_name,indicator);
}
