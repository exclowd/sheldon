//
// Created by kannav on 9/1/20.
//

#ifndef SHELDON_COMMAND_H
#define SHELDON_COMMAND_H

#include <stdlib.h>
#include <string.h>

typedef struct NODE {
    char * text;
    int flag;
} node;

typedef struct LINK_LIST_NODE {
    struct LINK_LIST_NODE * next;
    node* word;
} list_node;

// Yay malloc went away
typedef struct SIMPLE_COMMAND {
    int flag; // is bg or fg

    node * name;

    list_node * args;

} simple_command;

simple_command * load_command(char * line);

void free_command(simple_command * command);

char ** generate_argv (list_node *list, int starting_index);

extern list_node *current; // the current list node

extern list_node *nonopt; // start the execution from here

extern char * option_argument;

int getcommand_opt(list_node *list, char *opts);

void reset_getcommand_opt(void);

#endif //SHELDON_COMMAND_H
