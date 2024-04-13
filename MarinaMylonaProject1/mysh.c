#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>

#include "pipes.h"
#include "alias.h"
#include "history.h"


#define MAX_LINE 512
#define MAX_ARGS 20


int main(int argc, char* argv[]) {
    char *line = malloc(sizeof(char*) * MAX_LINE); //the command given by the user
    int line_size;
    char *token[MAX_ARGS]; //words or special characters
    char *args_array[MAX_ARGS];

    int i = 0;
    int in_redirection = 0;
    int out_redirection = 0;
    int double_redirection = 0;
    char *in_file;
    char *out_file;
    int input;
    int output;
    int pid;

    int pipes_file[2];
    char *args_pipe_array[MAX_ARGS]; //right side from pipe
    int pipe_flag = 0;
    int pipe_pos;

    int alias_flag = 0;
    count_alias = 0;

    count_history = 0;
    while (1) {
        //initialization
        in_file = NULL;
        out_file = NULL;
        out_redirection = 0;
        in_redirection = 0;
        double_redirection = 0;
        pipe_flag = 0;
        pipe_pos = 0;

        printf("mysh> ");
        fflush(stdout);

        fgets(line, MAX_LINE, stdin);
        line_size = strlen(line) - 1;
        if (line[line_size] == '\n')
            line[line_size] = '\0';

        //check for exit
        if (strcmp(line, "exit") == 0) {
            break;
        }

        //line replacement with alias
        if(alias_flag) {
                for (i = 0; i < count_alias; i++)
                    if (strcmp(line, alias_array[i].name) == 0)
                            strcpy(line, alias_array[i].command);
        }


        char helper_line[MAX_LINE];
        //to use the original line in another case
        strcpy(helper_line,line);

        //seperate token from the line
        token[0] = strtok(helper_line, " ");
        i = 0;
        while (token[i] != NULL) {
            i++;
            token[i] = strtok(NULL, " ");
        }
        int num_tokens = i;

        //check for cd
        if(strcmp(token[0], "cd") == 0){
            //if line is cd
            if(num_tokens == 1) {
                if (chdir(getenv("HOME")) == -1) {
                    perror("chdir");
                    return 1;
                }
            }
            //if line is cd and something else
            else if(chdir(token[1]) == -1){
                perror("chdir");
                return 1;
            }
            add_history(line);
            continue;
        }

        //check for only myHistory
        if(strcmp(line, "myHistory") == 0) {
            print_history();
            continue;
        }
        //if line is myHistory and a number
        else if(strcmp(token[0], "myHistory") == 0) {
                num_tokens = find_history(token,line);
                if(num_tokens == -1)
                    continue;
        }
        else
            add_history(line);

        //check for alias
        if(strcmp(token[0], "createalias") == 0){
            alias_flag = 1;
            count_alias++;
            if(!create_alias(token,num_tokens)){
                printf("Could not create alias\n");
                count_alias--;
            }
            continue;
        }


        if(strcmp(token[0], "destroyalias") == 0){
            if(destroy_alias(token)) {
                count_alias--;
                //if you destroy all aliases
                if (count_alias == 0)
                    alias_flag = 0;
            }
            else
                printf("No alias found with name:%s\n",token[1]);
            continue;
        }

        i = 0;
        int num_of_args = 0;
        while (token[i] != NULL) {

//          check for redirection
            if (strcmp(token[i], "<") == 0) {
                in_file = token[i + 1];
                in_redirection = 1;
            }
            else if (strcmp(token[i], ">") == 0) {
                out_file = token[i + 1];
                out_redirection = 1;
                i++;
                break;
            }
            else if (strcmp(token[i], ">>") == 0) {
                out_file = token[i + 1];
                double_redirection = 1;
                break;
            }

//          check for pipes
            else if (strcmp(token[i], "|") == 0) {
                int num_of_pipes_args = 0;
                pipe_flag = 1;
                args_pipe_array[pipe_pos] = token[i + 1];
                pipe_pos++;
                num_of_pipes_args++;
                i++;
            } else {
                args_array[num_of_args] = token[i];
                num_of_args++;
            }
            i++;
        }

        args_array[num_of_args] = NULL;
        args_pipe_array[num_of_args] = NULL;


        pid = fork();
        if (pid == -1) {
            perror("fork");
            return 1;
        } else if (pid == 0) {
            //child pid process
            if (pipe_flag) {
                pipe_function(pipes_file,args_array, args_pipe_array, in_redirection,out_redirection, input, in_file, output, out_file);
            }
            else {
                if (in_redirection) {
                    input = open(in_file, O_RDONLY | O_CREAT, 0777);
                    if (input == -1) {
                        printf("open\n");
                        return 1;
                    }
                    int file4 = dup2(input, STDIN_FILENO);
                    if(file4 == -1){
                        perror("dup2");
                        return 1;
                    }
                    if(close(input) == -1){
                        perror("close");
                        return 1;
                    }
                }

                //redirect if needed
                if (out_redirection) {
                    output = open(out_file, O_WRONLY | O_CREAT | O_TRUNC, 0777);
                    if (output == -1) {
                        printf("open output\n");
                        return 1;
                    }
                    int file2 = dup2(output, STDOUT_FILENO);
                    if (file2 == -1) {
                        perror("dup2");
                        return 1;
                    }
                    if(close(output) == -1){
                        perror("close");
                        return 1;
                    }
                }
                if (double_redirection) {
                    output = open(out_file, O_WRONLY | O_APPEND | O_CREAT, 0777);
                    if (output == -1) {
                        printf("open output\n");
                        return 1;
                    }
                    int file1 = dup2(output, STDOUT_FILENO);
                    if(file1 == -1){
                        perror("dup2");
                        return 1;
                    }
                    if(close(output) == -1){
                        perror("close");
                        return 1;
                    }
                }

                //run the line
                int err = execvp(args_array[0], args_array);
                if (err == -1) {
                    perror("execvp1");
                    return 1;
                }

            }
        }
        else {
            //parent pid process
            int status;
            wait(&status);

            if (WIFEXITED(status)) {
                //if the process is finished correctly
                int statusCode = WEXITSTATUS(status);
                if (statusCode != 0)
                    printf("FAIL %d\n", statusCode);
                else {
                    printf("SUCCESS!\n");
                }
            }

        }
    }
    //free section
    free(line);

    for(i=0;i<count_alias;i++){
        free(alias_array[i].name);
        free(alias_array[i].command);
    }
    free(alias_array);

    return 0;
}