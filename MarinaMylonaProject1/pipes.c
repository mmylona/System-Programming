#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/wait.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include "pipes.h"


void pipe_function(int pipes_file[2], char *args_array[], char *args_pipe_array[], int in_redirection, int out_redirection, int input, char* in_file, int output, char* out_file){
    if (pipe(pipes_file) == -1){
        perror("pipe");
        exit(1);
    }
    pid_pipe = fork();

    if (pid_pipe < 0) {
        perror("fork");
        exit(1);
    }
    else if (pid_pipe == 0) {
//        child pid_pipe process
        if (close(pipes_file[1]) == -1) {
            perror("close");
            exit(1);
        }
        //read from pipe
        int file3 = dup2(pipes_file[0], STDIN_FILENO);
        if (file3 == -1) {
            perror("dup2");
           exit(1);
        }
        if (close(pipes_file[0]) == -1) {
            perror("close");
            exit(1);
        }

        if (out_redirection) {
            output = open(out_file, O_WRONLY | O_CREAT | O_TRUNC, 0777);
            if (output == -1) {
                perror("open");
                exit(1);
            }

            int file2 = dup2(output, STDOUT_FILENO);
            if (file2 == -1) {
                perror("dup2");
                exit(1);
            }
            if (close(output) == -1) {
                perror("close");
                exit(1);
            }
        }

        int err = execvp(args_pipe_array[0], args_pipe_array);
        if (err == -1) {
            perror("execvp");
            exit(1);
        }
    }
    else if(pid_pipe > 0){
        //parent pid_pipe process
        if(in_redirection){
            input = open(in_file, O_RDONLY | O_CREAT, 0777);
            if (input == -1) {
                perror("open");
                exit(1);
            }
            int file4 = dup2(input, STDIN_FILENO);
            if(file4 == -1){
                perror("dup2");
                exit(1);
            }
            close(input);
        }

        if (close(pipes_file[0]) == -1) {
            perror("close");
            exit(1);
        }
        //write in the pipe
        int file6 = dup2(pipes_file[1], STDOUT_FILENO);
        if (file6 == -1) {
            perror("dup2");
            exit(1);
        }
        if (close(pipes_file[1]) == -1) {
            perror("close");
            exit(1);
        }

        int err = execvp(args_array[0], args_array);
        if (err == -1) {
            perror("execvp");
            exit(1);
        }
    }
}







