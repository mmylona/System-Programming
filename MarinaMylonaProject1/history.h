#ifndef UNTITLED1_HISTORY_H
#define UNTITLED1_HISTORY_H

#define MAX_LINE 512

struct history{
    char array[MAX_LINE];
    int id;
};

struct history history_array[20];
int count_history;


void print_history();

int find_history(char *token[], char *line);

void add_history(char *line);

#endif


