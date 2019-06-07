#include "list.h"
#include "initializer.h"
#include <iostream>
#include <cerrno>
#include <cstring>

using namespace std;

void ctrl_list_processing(const char *shm_name){
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
}

void ctrl_list_waiting(const char *shm_name){
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
  cout << "Waiting:" << endl;
  if(shResources->shInput.current == 0) return;
  custom_input = shResources->shInput.maximun;
  for (int i = 0; i < custom_input; i++) {
    custom_input_length = shResources->shInput.Inboxes[i].maximun;
    for (int j = 0; j < custom_input_length; j++) {
      exam current_exam = shResources->shInput.Inboxes[i].exams[j];
      if( current_exam.waiting){
        cout << "["
        << current_exam.id << " "
        << current_exam.inbox << " "
        << current_exam.sample << " "
        << current_exam.quantity << "]"
        << endl;
      }
    }
  }
  close(sm);
}

void ctrl_list_reported(const char *shm_name){
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
  cout << "Reported:" << endl;
  if(shResources->shInput.current == 0) return;
  custom_output = shResources->shOutput.maximun;
  for (int i = 0; i < custom_output; i++) {
    exam result = shResources->shOutput.exams_ready[i];
    if( result.reported){
      cout << "["
      << result.id << " "
      << result.inbox << " "
      << result.sample << " "
      << result.results << "]"
      << endl;
    }
  }
  close(sm);
}

void ctrl_list_reactive(const char *shm_name){
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
}
