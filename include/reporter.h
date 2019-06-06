#pragma once
#include "default.h"
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <semaphore.h>
#include "initializer.h"
#include <unistd.h>

void reporter(int params_length,char* params[]);
