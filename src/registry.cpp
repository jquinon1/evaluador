#include "registry.h"
#include <iostream>
#include <cerrno>
#include <cstring>
#include "helper.h"
#include <unistd.h>
#include "initializer.h"
#include <fstream>

using namespace std;
// Define allowed options
const int registry_number_parameters = 1;
const char *registry_parameters[] = {"-n"};
const char *repo_name = "evaluador";

void interactive_registry(const char* shm_name){
  // Getting shm
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
  // Parsing interactive elements
  string input;
  char delimiter = ' ';
  string metadata[3];
  getline(cin,input);
  while ( input != "EOF") {
    // std::cout << "your line" << input << '\n';
    for (int i = 0; i < 3; i++) {
      string data = input.substr(0,input.find(delimiter)).c_str();
      input = input.substr(input.find(delimiter)+1);
      metadata[i] = data;
    }
    int inbox = atoi(metadata[0].c_str());
    char sample_type = (char)metadata[1][0];
    int sample_quantity = atoi(metadata[2].c_str());
    // Opening semaphores
    string input_empty_name = string(shm_name) + "_inbox_" + to_string(inbox) + "_empty";
    string input_full_name = string(shm_name) + "_inbox_" + to_string(inbox) + "_full";
    string input_mutex_name = string(shm_name) + "_inbox_" + to_string(inbox) + "_mutex";
    if ((shResources->shInput.Inboxes[inbox].empty = sem_open(input_empty_name.c_str(), 0)) == SEM_FAILED){
      exit(EXIT_FAILURE);
    }
    if ((shResources->shInput.Inboxes[inbox].full = sem_open(input_full_name.c_str(), 0)) == SEM_FAILED){
      exit(EXIT_FAILURE);
    }
    if ((shResources->shInput.Inboxes[inbox].mutex = sem_open(input_mutex_name.c_str(), 0)) == SEM_FAILED){
      exit(EXIT_FAILURE);
    }
    // Get the id for the exam
    string input_semaphore_name = string(shm_name) + "_general_input_mutex";
    shResources->shInput.mutex = sem_open(input_semaphore_name.c_str(), 0);
    sem_wait(shResources->shInput.mutex);
    int exam_id = shResources->shInput.current;
    shResources->shInput.current++;
    sem_post(shResources->shInput.mutex);
    // Create exam
    sem_wait(shResources->shInput.Inboxes[inbox].empty);
    sem_wait(shResources->shInput.Inboxes[inbox].mutex);
    int position = shResources->shInput.Inboxes[inbox].in;
    shResources->shInput.Inboxes[inbox].exams[position].id = exam_id;
    shResources->shInput.Inboxes[inbox].exams[position].processing = false;
    shResources->shInput.Inboxes[inbox].exams[position].waiting = true;
    shResources->shInput.Inboxes[inbox].exams[position].reported = false;
    shResources->shInput.Inboxes[inbox].exams[position].sample = sample_type;
    shResources->shInput.Inboxes[inbox].exams[position].quantity = sample_quantity;
    shResources->shInput.Inboxes[inbox].in = (shResources->shInput.Inboxes[inbox].in + 1) % shResources->shInput.Inboxes[inbox].maximun;
    shResources->shInput.Inboxes[inbox].current++;
    sem_post(shResources->shInput.Inboxes[inbox].mutex);
    sem_post(shResources->shInput.Inboxes[inbox].full);

    // string sample_id = string(shm_name)+"_IN_"+to_string(inbox)+"_"+to_string(position);
    cout << exam_id << endl;
    getline(cin,input);
  }
  close(sm);
}
// Taken from stackoverflow
bool file_exists(const std::string& name) {
  struct stat buffer;
  return (stat (name.c_str(), &buffer) == 0);
}
// https://stackoverflow.com/questions/12774207/fastest-way-to-check-if-a-file-exist-using-standard-c-c11-c

void file_registry(const char* shm_name, bool default_shn, char* parameters[],int parameters_length){
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
  // Parsing files
  int start = (default_shn) ? 2 : 4;
  for (int i = start; i < parameters_length; i++) {
    if( file_exists(string(parameters[i]) ) ) {
      // Get the directory
      char *current_location = get_current_dir_name();
      string outputfile = string(current_location).substr(0,string(current_location).find(repo_name)+strlen(repo_name)) + "/examples/" + string(parameters[i]).substr(0,string(parameters[i]).rfind('.')) + ".spl";
      ifstream infile(parameters[i]);
      ofstream outfile(outputfile);
      string input;
      char delimiter = ' ';
      string metadata[3];
      while (getline(infile, input)){
        std::cout << input << '\n';
        for (int i = 0; i < 3; i++) {
          string data = input.substr(0,input.find(delimiter)).c_str();
          input = input.substr(input.find(delimiter)+1);
          metadata[i] = data;
        }
        int inbox = atoi(metadata[0].c_str());
        char sample_type = (char)metadata[1][0];
        int sample_quantity = atoi(metadata[2].c_str());
        // Opening semaphores
        string input_empty_name = string(shm_name) + "_inbox_" + to_string(inbox) + "_empty";
        string input_full_name = string(shm_name) + "_inbox_" + to_string(inbox) + "_full";
        string input_mutex_name = string(shm_name) + "_inbox_" + to_string(inbox) + "_mutex";
        if ((shResources->shInput.Inboxes[inbox].empty = sem_open(input_empty_name.c_str(), 0)) == SEM_FAILED){
          exit(EXIT_FAILURE);
        }
        if ((shResources->shInput.Inboxes[inbox].full = sem_open(input_full_name.c_str(), 0)) == SEM_FAILED){
          exit(EXIT_FAILURE);
        }
        if ((shResources->shInput.Inboxes[inbox].mutex = sem_open(input_mutex_name.c_str(), 0)) == SEM_FAILED){
          exit(EXIT_FAILURE);
        }
        // Get the id for the exam
        string input_semaphore_name = string(shm_name) + "_general_input_mutex";
        shResources->shInput.mutex = sem_open(input_semaphore_name.c_str(), 0);
        sem_wait(shResources->shInput.mutex);
        int exam_id = shResources->shInput.current;
        shResources->shInput.current++;
        sem_post(shResources->shInput.mutex);
        // Create exam
        sem_wait(shResources->shInput.Inboxes[inbox].empty);
        sem_wait(shResources->shInput.Inboxes[inbox].mutex);
        int position = shResources->shInput.Inboxes[inbox].in;
        shResources->shInput.Inboxes[inbox].exams[position].id = exam_id;
        shResources->shInput.Inboxes[inbox].exams[position].processing = false;
        shResources->shInput.Inboxes[inbox].exams[position].waiting = true;
        shResources->shInput.Inboxes[inbox].exams[position].reported = false;
        shResources->shInput.Inboxes[inbox].exams[position].sample = sample_type;
        shResources->shInput.Inboxes[inbox].exams[position].quantity = sample_quantity;
        shResources->shInput.Inboxes[inbox].in = (shResources->shInput.Inboxes[inbox].in + 1) % shResources->shInput.Inboxes[inbox].maximun;
        shResources->shInput.Inboxes[inbox].current++;
        sem_post(shResources->shInput.Inboxes[inbox].mutex);
        sem_post(shResources->shInput.Inboxes[inbox].full);

        // string sample_id = string(shm_name)+"_IN_"+to_string(inbox)+"_"+to_string(position);
        outfile << exam_id << endl;
      }
      outfile.close();
      infile.close();
    }
  }
  close(sm);
}

void registry(int params_length,char *params[]) {
  bool default_sh_name = true;
  int index;
  if((index = param_index(params_length,params,(char *)"-n")) != -1 ) {
    shared_mem_name = params[index+1];
    default_sh_name = false;
  }
  // Figure out if interactive or file
  if( (default_sh_name && params_length == 2) || (!default_sh_name && params_length == 4) ){
    interactive_registry(shared_mem_name);
  }

  if( (default_sh_name && params_length > 2) || (!default_sh_name && params_length > 4) ){
    file_registry(shared_mem_name,default_sh_name,params,params_length);
  }
}
