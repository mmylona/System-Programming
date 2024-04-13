#include <stdio.h>
#include <string.h>
#include "history.h"


void print_history(){
    if (count_history == 0)
        printf("No history\n");

    for (int i = 0; i < count_history; i++)
        printf("%d: %s\n", history_array[i].id, history_array[i].array);
}

int find_history(char *token[], char *line){
    //change char to int
    int number = (int)*token[1] - '0';
    if(number > count_history || number < 1){
        printf("No history with number %d\n",number);
        return -1;
    }
    //change the line
    strcpy(line,history_array[number-1].array);
    int line_size = strlen(line) - 1;
    if (line[line_size] == '\n')
        line[line_size] = '\0';

    //seperate token from the line
    token[0] = strtok(line, " ");
    int i = 0;
    while (token[i] != NULL) {
        i++;
        token[i] = strtok(NULL, " ");
    }
    //return the num_tokens
    return i;
}

void add_history(char *line){
    //if history is full
    if(count_history == 20){
        for(int i=0;i<20;i++){
            //move back one position
            strcpy(history_array[i].array,history_array[i+1].array);
        }
        //copy the line to the last history struct
        strcpy(history_array[19].array,line);
    }
    else {
        strcpy(history_array[count_history].array, line);
        history_array[count_history].id = count_history + 1;
        count_history++;

    }
}

