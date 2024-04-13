#ifndef UNTITLED1_PIPES_H
#define UNTITLED1_PIPES_H

int pid_pipe;

void pipe_function(int pipes_file[2], char *args_array[], char *args_pipe_array[], int in_redirection, int out_redirection, int input, char* in_file, int output, char* out_file);


#endif