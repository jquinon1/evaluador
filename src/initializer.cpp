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
#include <pthread.h>
#include <thread>

using namespace std;

// Define allowed options
const int init_number_parameters = 8;
const char *init_parameters[] = {"-i","-ie","-oe","-n","-b","-d","-s","-q"};
// Initialize intern queues
// 0 -> B
// 1 -> D
// 2 -> S
struct intern_queue samples_type[SAMPLES_TYPE];

void* evaluator(void *arg){
  struct thread_args *arguments = (struct thread_args *)arg;
  int inbox = arguments->inbox_to_lister;
  string temp_name = "/" + string(arguments->shm_name);
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
  sem_t *intern_empty, *intern_full, *intern_mutex, *output_mutex, *output_full, *output_empty;
  string intern_empty_name = string(arguments->shm_name) + "_intern_" + to_string(inbox) + "_empty";
  string intern_full_name = string(arguments->shm_name) + "_intern_" + to_string(inbox) + "_full";
  string intern_mutex_name = string(arguments->shm_name) + "_intern_" + to_string(inbox) + "_mutex";
  intern_empty = sem_open(intern_empty_name.c_str(), 0);
  intern_full = sem_open(intern_full_name.c_str(), 0);
  intern_mutex = sem_open(intern_mutex_name.c_str(), 0);
  string output_empty_name = string(arguments->shm_name) + "_output_empty";
  string output_full_name = string(arguments->shm_name) + "_output_full";
  string output_mutex_name = string(arguments->shm_name) + "_output_mutex";
  output_empty = sem_open(output_empty_name.c_str(), 0);
  output_full = sem_open(output_full_name.c_str(), 0);
  output_mutex = sem_open(output_mutex_name.c_str(), 0);
  exam returned = {};
  while (true) {
    sem_wait(intern_full);
    sem_wait(intern_mutex);
    // Getting the copy of the exam
    int copied_exam_out = shResources->copy_intern[inbox].out;
    struct exam copied_exam = shResources->copy_intern[inbox].exams[copied_exam_out];
    shResources->copy_intern[inbox].out = (copied_exam_out + 1) % shResources->copy_intern[inbox].maximun;
    shResources->copy_intern[inbox].current--;
    int get = samples_type[inbox].out;
    shResources->copy_intern[inbox].exams[copied_exam_out].processing = false;
    // here change this for a method which return the sample processed
    returned = samples_type[inbox].exams[get];
    samples_type[inbox].exams[get].processing = false;
    samples_type[inbox].out = (get + 1) % samples_type[inbox].maximun;
    samples_type[inbox].current--;
    sem_post(intern_mutex);
    sem_post(intern_empty);
    // Putting the examn in the evaluating list (shared)
    int random;
    switch (inbox) {
      case 0:
        random = 1 + ( std::rand() % ( 7 - 1 + 1 ) );
        break;
      case 1:
        random = 5 + ( std::rand() % ( 20 - 5 + 1 ) );
        break;
      case 2:
        random = 8 + ( std::rand() % ( 25 - 8 + 1 ) );
        break;
      default:
        exit(EXIT_FAILURE);
    }
    shResources->evaluating[inbox] = copied_exam;
    std::cout << inbox << '\n';
    this_thread::sleep_for (std::chrono::seconds(random));
    // processing the sample
    // Put the processed sample in output
    sem_wait(output_empty);
    sem_wait(output_mutex);
    shResources->evaluating[inbox].processing = false;
    int output_position_in = shResources->shOutput.in;
    shResources->shOutput.exams_ready[output_position_in] = returned;
    shResources->shOutput.exams_ready[output_position_in].reported = true;
    shResources->shOutput.exams_ready[output_position_in].waiting = false;
    shResources->shOutput.exams_ready[output_position_in].processing = false;
    shResources->shOutput.in = (output_position_in + 1) % shResources->shOutput.maximun;
    shResources->shOutput.current++;
    sem_post(output_mutex);
    sem_post(output_full);
    std::cout << "Gathered exam: " << returned.id <<'\n';
  }
  return NULL;
}

void* pre_evaluator(void *arg){
  struct thread_args *arguments = (struct thread_args *)arg;
  int inbox = arguments->inbox_to_lister;
  string temp_name = "/" + string(arguments->shm_name);
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
  // Opening semaphores
  string input_empty_name = string(arguments->shm_name) + "_inbox_" + to_string(inbox) + "_empty";
  string input_full_name = string(arguments->shm_name) + "_inbox_" + to_string(inbox) + "_full";
  string input_mutex_name = string(arguments->shm_name) + "_inbox_" + to_string(inbox) + "_mutex";
  sem_t *thread_empty, *thread_full, *thread_mutex, *intern_empty, *intern_full, *intern_mutex;
  thread_empty = sem_open(input_empty_name.c_str(), 0);
  thread_full = sem_open(input_full_name.c_str(), 0);
  thread_mutex = sem_open(input_mutex_name.c_str(), 0);
  struct exam exam = {};
  while (true) {
    sem_wait(thread_full);
    sem_wait(thread_mutex);
    int exam_position = shResources->shInput.Inboxes[inbox].out;
    // getting exam
    exam = shResources->shInput.Inboxes[inbox].exams[exam_position];
    shResources->shInput.Inboxes[inbox].exams[exam_position].waiting = false;
    shResources->shInput.Inboxes[inbox].out = (exam_position + 1) % shResources->shInput.Inboxes[inbox].maximun;
    shResources->shInput.Inboxes[inbox].current--;
    clock_gettime(CLOCK_REALTIME,&exam.processing_time);
    sem_post(thread_mutex);
    sem_post(thread_empty);
    int sample_type = (exam.sample == 'B') ? 0 : (exam.sample == 'D') ? 1 : 2;
    string intern_empty_name = string(arguments->shm_name) + "_intern_" + to_string(sample_type) + "_empty";
    string intern_full_name = string(arguments->shm_name) + "_intern_" + to_string(sample_type) + "_full";
    string intern_mutex_name = string(arguments->shm_name) + "_intern_" + to_string(sample_type) + "_mutex";
    intern_empty = sem_open(intern_empty_name.c_str(), 0);
    intern_full = sem_open(intern_full_name.c_str(), 0);
    intern_mutex = sem_open(intern_mutex_name.c_str(), 0);
    sem_wait(intern_empty);
    sem_wait(intern_mutex);
    // Copying only not relevant info to shared memory to be able to list processing samples
    int copy_entry_point = shResources->copy_intern[sample_type].in;
    shResources->copy_intern[sample_type].exams[copy_entry_point] = exam;
    shResources->copy_intern[sample_type].exams[copy_entry_point].processing = true;
    shResources->copy_intern[sample_type].exams[copy_entry_point].waiting = false;
    shResources->copy_intern[sample_type].exams[copy_entry_point].reported = false;
    shResources->copy_intern[sample_type].in = (copy_entry_point + 1) % shResources->copy_intern[sample_type].maximun;
    shResources->copy_intern[sample_type].current++;
    // Putting the exam in the intern queue
    int entry_pos = samples_type[sample_type].in;
    samples_type[sample_type].exams[entry_pos] = exam;
    samples_type[sample_type].exams[entry_pos].processing = true;
    samples_type[sample_type].exams[entry_pos].waiting = false;
    samples_type[sample_type].exams[entry_pos].reported = false;
    samples_type[sample_type].in = (entry_pos + 1) % samples_type[sample_type].maximun;
    samples_type[sample_type].current++;
    sem_post(intern_mutex);
    sem_post(intern_full);
  }
  close(sm);
  return NULL;
}
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
  shResources->shInput.current = 0;
  shResources->shInput.maximun = custom_input;
  string input_semaphore_name = string(shm_name) + "_general_input_mutex";
  sem_open(input_semaphore_name.c_str(),O_CREAT | O_EXCL, 0660,1);
  for (int i = 0; i < custom_input; i++) {
    // Create the required semaphores for each inbox
    string input_empty_name = string(shm_name) + "_inbox_" + to_string(i) + "_empty";
    string input_full_name = string(shm_name) + "_inbox_" + to_string(i) + "_full";
    string input_mutex_name = string(shm_name) + "_inbox_" + to_string(i) + "_mutex";
    shResources->shInput.Inboxes[i].in = 0;
    shResources->shInput.Inboxes[i].out = 0;
    shResources->shInput.Inboxes[i].current = 0;
    shResources->shInput.Inboxes[i].maximun = custom_input_length;
    sem_open(input_empty_name.c_str(),O_CREAT | O_EXCL, 0660, custom_input_length);
    sem_open(input_full_name.c_str(),O_CREAT | O_EXCL, 0660, 0);
    sem_open(input_mutex_name.c_str(),O_CREAT | O_EXCL, 0660, 1);
  }
  // Create the required semaphores for output
  string output_empty_name = string(shm_name) + "_output_empty";
  string output_full_name = string(shm_name) + "_output_full";
  string output_mutex_name = string(shm_name) + "_output_mutex";
  shResources->shOutput.in = 0;
  shResources->shOutput.out = 0;
  shResources->shOutput.current = 0;
  shResources->shOutput.maximun = custom_output;
  sem_open(output_empty_name.c_str(),O_CREAT | O_EXCL, 0660, custom_output);
  sem_open(output_full_name.c_str(),O_CREAT | O_EXCL, 0660, 0);
  sem_open(output_mutex_name.c_str(),O_CREAT | O_EXCL, 0660, 1);
  for (int i = 0; i < SAMPLES_TYPE; i++) {
    shResources->copy_intern[i].in = 0;
    shResources->copy_intern[i].out = 0;
    shResources->copy_intern[i].current = 0;
    shResources->copy_intern[i].maximun = custom_intern_queues;
  }
  // Storing needed vars
  shResources->reactive_blood = custom_reactive_blood;
  shResources->reactive_detritos = custom_reactive_detritos;
  shResources->reactive_skin = custom_reactive_skin;

  close(sm);
  // Fill intern queues values
  for (int i = 0; i < SAMPLES_TYPE; i++) {
    string intern_empty_name = string(shm_name)+ "_intern_" + to_string(i)  + "_empty";
    string intern_full_name = string(shm_name) + "_intern_" + to_string(i) + "_full";
    string intern_mutex_name = string(shm_name)+ "_intern_" + to_string(i)  + "_mutex";
    sem_open(intern_empty_name.c_str(),O_CREAT | O_EXCL, 0660, custom_intern_queues);
    sem_open(intern_full_name.c_str(),O_CREAT | O_EXCL, 0660, 0);
    sem_open(intern_mutex_name.c_str(),O_CREAT | O_EXCL, 0660, 1);
    samples_type[i].in = 0;
    samples_type[i].out = 0;
    samples_type[i].current = 0;
    samples_type[i].maximun = custom_intern_queues;
  }
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
  // struct timespec tm;
  // clock_gettime(CLOCK_REALTIME, &tm);
  // std::cout << tm.tv_sec << '\n';
  // sleep(10);
  // struct timespec tms;
  // clock_gettime(CLOCK_REALTIME, &tms);
  // std::cout << (tms.tv_sec - tm.tv_sec) << '\n';
  create_shm(shared_mem_name);
  // Launch evaluators
  const int MAX_THREADS = 100;
  pthread_t pre_evaluators[MAX_THREADS];
  pthread_t evaluators[SAMPLES_TYPE];
  // sleep(30);
  for (int i = 0; i < custom_input; i++) {
    thread_args *args = (thread_args *)malloc(sizeof(thread_args));
    args->shm_name = (char*)shared_mem_name;
    args->inbox_to_lister = i;
    pthread_create(( pre_evaluators + i ), NULL, pre_evaluator, (void *)args);
  }
  for (int i = 0; i < SAMPLES_TYPE; i++) {
    thread_args *args = (thread_args *)malloc(sizeof(thread_args));
    args->shm_name = (char*)shared_mem_name;
    args->inbox_to_lister = i;
    pthread_create(( evaluators + i ), NULL, evaluator, (void *)args);
  }
  for (int i = 0; i < custom_input; i++) {
    pthread_join(pre_evaluators[i],NULL);
  }
}
