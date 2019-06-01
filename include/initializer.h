#pragma once
#include <fcntl.h>
#include <sys/stat.h>
#include <semaphore.h>
#include "default.h"

struct exam{
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
  struct exam exams[DEFAULT_INPUT_LENGHT];
};

struct Input{
  int in;
  int out;
  int maximun;
  int current;
  struct Inbox Inboxes[DEFAULT_INPUT];
};

void initializer(int params_lenght,char* params[]);
