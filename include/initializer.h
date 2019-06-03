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
  char sample;
  int quantity;
};

struct Inbox{
  sem_t *full;
  sem_t *empty;
  sem_t *mutex;
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
  sem_t *full;
  sem_t *empty;
  sem_t *mutex;
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

void initializer(int params_length,char* params[]);
