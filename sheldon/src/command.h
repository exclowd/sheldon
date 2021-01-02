//
// Created by kannav on 9/1/20.
//

#ifndef SHELDON_SRC_COMMAND_H
#define SHELDON_SRC_COMMAND_H

typedef struct LINK_LIST_NODE {
  char *_text;
  struct LINK_LIST_NODE *_next;
} __attribute__((aligned(16))) Argument;

typedef Argument ArgsList;

// Yay malloc went away
typedef struct SIMPLE_COMMAND {
  char *_name;
  ArgsList *_args;
} __attribute__((aligned(16))) SimpleCommand;

typedef struct LINK_LIST_SIMPLE_COMMAND {
  SimpleCommand *_command;
  struct LINK_LIST_SIMPLE_COMMAND *_next;
} __attribute__((aligned(16))) SimpleCommandList;

typedef struct COMMAND {
  char *_outFile;
  char *_inputFile;
  int _append_input;
  SimpleCommandList *_simple_commands;
  int _background;
} __attribute__((aligned(32))) __attribute__((packed)) CompoundCommand;

CompoundCommand *parser(char *line);

extern CompoundCommand *current_command;

extern SimpleCommand *current_simple_command;

int len(ArgsList *list);

void free_compound_command(CompoundCommand *command);

char *get_complete_command(char *command, ArgsList *args);

char **generate_argv(char *command, ArgsList *list, int starting_index);

extern ArgsList *current; // the current list word

extern ArgsList *nonopt; // start the execution from here

extern char *option_argument;

int get_command_opt(ArgsList *list, char *opts);

void reset_get_command_opt(void);

#endif //SHELDON_SRC_COMMAND_H
