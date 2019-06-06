#pragma once
#include "default.h"
#include "helper.h"
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

void ctrl_list_processing(const char *shm_name);
void ctrl_list_waiting(const char *shm_name);
void ctrl_list_reported(const char *shm_name);
void ctrl_list_reactive(const char *shm_name);
