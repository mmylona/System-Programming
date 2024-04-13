#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "alias.h"


int create_alias(char* token[], int num_tokens){
    if(num_tokens > 2){
        char *alias_list = NULL;
        alias_list = (char *) malloc(strlen(token[2] + 1));
        if (alias_list == NULL) {
            perror("malloc");
            return 0;
        }
        //copy the first token from the alias
        strcpy(alias_list, token[2]);

        if (num_tokens > 3) {
            //copy the rest tokens if exists
            for (int i = 3; i < num_tokens; i++) {
                alias_list = (char *) realloc(alias_list, strlen(token[i] + 1));
                strcat(alias_list, " ");
                strcat(alias_list, token[i]);
            }
        }

        //move the " from string
        alias_list[0] = ' ';
        int alias_list_size = strlen(alias_list) - 1;
        alias_list[alias_list_size]= ' ';

        for (int i = 1; i < count_alias; i++) {
            //check if there is an alias with the same name
            if (strcmp(alias_array[i-1].name, token[1]) == 0) {
                count_alias--;
                if(!destroy_alias(token)) {
                    count_alias++;
                    perror("destroy");
                    free(alias_list);
                    return 0;
                }
            }
        }

        //put new alias in the struct
        alias_array = realloc(alias_array,sizeof(struct alias) * count_alias);
        if (alias_array == NULL) {
            perror("realloc");
            free(alias_list);
            return 0;
        }

        alias_array[count_alias - 1].name = (char*)malloc(strlen(token[1]) + 1);
        if (alias_array[count_alias - 1].name == NULL) {
            perror("malloc");
            free(alias_list);
            return 0;
        }
        alias_array[count_alias - 1].command = malloc(sizeof(alias_list) + 1);
        if (alias_array[count_alias - 1].command == NULL) {
            perror("malloc");
            free(alias_list);
            return 0;
        }
        strcpy(alias_array[count_alias - 1].name, token[1]);
        strcpy(alias_array[count_alias - 1].command, alias_list);

        free(alias_list);
        return 1;

    }
    printf("need more arguments for creating alias\n");
    return 0;
}

int destroy_alias(char * token[]){
    for(int i=0;i<count_alias;i++){
        if(strcmp(token[1], alias_array[i].name) == 0){
            if(count_alias > 1) {
                //move alias one step back
                for (int j = i; j < count_alias-1; j++) {
                    alias_array[j].name = realloc(alias_array[j].name, sizeof(alias_array[j + 1].name));
                    if (alias_array[j].name == NULL) {
                        perror("realloc");
                        return 0;
                    }

                    alias_array[j].command = realloc(alias_array[j].command, sizeof(alias_array[j + 1].command));
                    if (alias_array[j].command == NULL) {
                        perror("realloc");
                        return 0;
                    }

                    strcpy(alias_array[j].name, alias_array[j + 1].name);
                    strcpy(alias_array[j].command, alias_array[j + 1].command);
                }
            }
            //and free the last one
            free(alias_array[count_alias-1].name);
            free(alias_array[count_alias-1].command);
            return 1;
            }
    }

    return 0;
}
