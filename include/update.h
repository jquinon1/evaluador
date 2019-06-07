#pragma once
#include "default.h"
#include "helper.h"
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

void update_blood(const char *shm_name, int value);
void update_detritus(const char *shm_name, int value);
void update_skin(const char *shm_name, int value);
