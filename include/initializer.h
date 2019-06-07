#pragma once
#include <fcntl.h>
#include <sys/stat.h>
#include <semaphore.h>
#include "default.h"

struct exam{
  int id;
  bool processing;
  bool waiting;
  bool reported;
  int inbox;
  char sample;
  int quantity;
  int results;
};

struct Inbox{
  int in;
  int out;
  int current;
  int maximun;
  struct exam exams[DEFAULT_INPUT_LENGTH];
};

struct Input{
  sem_t *mutex;
  int maximun;
  int current;
  struct Inbox Inboxes[DEFAULT_INPUT];
};

struct Output{
  int in;
  int out;
  int current;
  int maximun;
  struct exam exams_ready[DEFAULT_OUTPUT];
};

struct Resources{
  struct Input shInput;
  struct Output shOutput;
  int reactive_blood;
  int reactive_skin;
  int reactive_detritos;
};

struct intern_queue{
  int in;
  int out;
  int current;
  int maximun;
  struct exam exams[DEFAULT_INTERN_QUEUES];
};

struct thread_args{
  char *shm_name;
  int inbox_to_lister;
};

void initializer(int params_length,char* params[]);
