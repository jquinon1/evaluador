#pragma once
void usage(char* progname);

bool check_valid_param(const int num_params, const char *parameters[],char *argument);

int param_index(int num_params, char *parameters[], char *param);
