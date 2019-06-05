#include "controller.h"
#include <iostream>
#include "helper.h"
#include "list.h"

using namespace std;
// Define allowed options
const int ctrl_number_parameters = 1;
const char *ctrl_parameters[] = {"-n"};
// strcmp(argument,parameters[i]) == 0
void ctrl_list(const char *shm_name, const char *input){
  const char *options[] = {"processing","waiting","reported","reactive","all"};
  if(!check_valid_param(5,options,const_cast<char*>(input))){
    cerr << "Error: Unknown command: " << input <<endl;
    cerr << "Valid are [ processing | waiting | reported | reactive | all ]" << endl;
    exit(EXIT_FAILURE);
  }
  // processing:0, waiting: 1, reported: 2, reactive: 3, all: 4
  int sub_command = param_index(5,const_cast<char**>(options),const_cast<char*>(input));
  switch (sub_command) {
    case 0:
      ctrl_list_processing(shm_name);
      break;
    case 1:
      ctrl_list_waiting(shm_name);
      break;
    case 2:
      ctrl_list_reported(shm_name);
      break;
    case 3:
      ctrl_list_reactive(shm_name);
      break;
    case 4:
      ctrl_list_processing(shm_name);
      ctrl_list_waiting(shm_name);
      ctrl_list_reported(shm_name);
      ctrl_list_reactive(shm_name);
      break;
    default:
      break;
  }
}

void ctrl_update(const char *shm_name,const char *input){
  std::cout << input << '\n';
}

void interactive_controller(const char *shm_name){
  string input;
  char delimiter = ' ';
  getline(cin,input);
  while ( input != "EOF") {
    // Getting command
    string command = input.substr(0,input.find(delimiter)).c_str();
    input = input.substr(input.find(delimiter)+1);
    if (command != "list" && command != "update") {
      cerr << "Error: Unknown command: " << command <<endl;
      cerr << "Valid are [ list | update ]" << endl;
      exit(EXIT_FAILURE);
    }
    if (command == "list") ctrl_list(shm_name,input.c_str());
    if (command == "update") ctrl_update(shm_name,input.c_str());
    getline(cin,input);
  }
}

void controller(int params_length,char *params[]) {
  // Check if the number of params has sense
  if(params_length % 2 != 0 ) usage(params[0]);

  // Check if params are valid
  for (int i = 2; i < params_length; i+=2) {
    if(!check_valid_param(ctrl_number_parameters,ctrl_parameters,params[i])) usage(params[0]);
  }
  int index;
  if((index = param_index(params_length,params,(char *)"-n")) != -1 ) shared_mem_name = params[index+1];
  interactive_controller(shared_mem_name);
}
