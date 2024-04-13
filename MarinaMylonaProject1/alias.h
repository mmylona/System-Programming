#ifndef UNTITLED1_ALIAS_H
#define UNTITLED1_ALIAS_H

struct alias{
    char *name;
    char *command;
};

struct alias *alias_array;
int count_alias;

int destroy_alias(char * token[]);


int create_alias(char* token[], int num_tokens);



#endif

