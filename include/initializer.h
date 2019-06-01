#pragma once
#include <fcntl.h>
#include <sys/stat.h>
#include <semaphore.h>

#define DEFAULT_INPUT 5
#define DEFAULT_INPUT_LENGHT 6
#define DEFAULT_OUTPUT 10
#define DEFAULT_SHM_NAME "evaluator"
#define DEFAULT_REACTIVE_BLOOD 100
#define DEFAULT_REACTIVE_DETRITOS 100
#define DEFAULT_REACTIVE_SKIN 100
#define DEFAULT_INTERN_QUEUES 6

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
