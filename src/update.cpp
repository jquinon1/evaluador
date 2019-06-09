#include "update.h"
#include "initializer.h"
#include <iostream>
#include <cerrno>
#include <cstring>
#include <time.h>

using namespace std;

void update_blood(const char *shm_name, int value){
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
  sem_t *semaphore_mutex;
  string semaphore_mutex_name = string(shm_name)+ "_blood_mutex";
  semaphore_mutex = sem_open(semaphore_mutex_name.c_str(), 0);
  sem_wait(semaphore_mutex);
  shResources->reactive_blood += value;
  sem_post(semaphore_mutex);
  close(sm);
}

void update_detritus(const char *shm_name, int value){
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
  sem_t *semaphore_mutex;
  string semaphore_mutex_name = string(shm_name)+ "_detritos_mutex";
  semaphore_mutex = sem_open(semaphore_mutex_name.c_str(), 0);
  sem_wait(semaphore_mutex);
  shResources->reactive_detritos += value;
  sem_post(semaphore_mutex);
  close(sm);
}

void update_skin(const char *shm_name, int value){
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
  sem_t *semaphore_mutex;
  string semaphore_mutex_name = string(shm_name)+ "_skin_mutex";
  semaphore_mutex = sem_open(semaphore_mutex_name.c_str(), 0);
  sem_wait(semaphore_mutex);
  shResources->reactive_skin += value;
  sem_post(semaphore_mutex);
  close(sm);
}
