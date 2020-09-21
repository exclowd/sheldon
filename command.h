//
// Created by kannav on 9/1/20.
//

#ifndef SHELDON_COMMAND_H
#define SHELDON_COMMAND_H

#include <stdlib.h>
#include <string.h>

typedef struct NODE {
  char *_text;
} word;

typedef struct LINK_LIST_NODE {
  struct LINK_LIST_NODE *_next;
  word *_word;
} word_list;

// Yay malloc went away
typedef struct SIMPLE_COMMAND {
  word *_name;
  word_list *_args;
} simple_command;

typedef struct LINK_LIST_SIMPLE_COMMAND {
  simple_command *_command;
  struct LINK_LIST_SIMPLE_COMMAND *_next;
} simple_command_list;

typedef struct COMMAND {
  char *_outFile;
  char *_inputFile;
  int _append_input;
  simple_command_list *_simple_commands;
  int _background;
} compound_command;

compound_command *Parser(char *line);

extern compound_command *current_command;

void free_command(simple_command *command);

char **generate_argv(word *command, word_list *list, int starting_index);

extern word_list *current; // the current list word

extern word_list *nonopt; // start the execution from here

extern char *option_argument;

int get_command_opt(word_list *list, char *opts);

void reset_get_command_opt(void);

#endif //SHELDON_COMMAND_H
