//
// Created by kannav on 9/1/20.
//

#ifndef SHELDON_SRC_COMMAND_H
#define SHELDON_SRC_COMMAND_H

typedef struct NODE {
  char *_text;
} Word;

typedef struct LINK_LIST_NODE {
  struct LINK_LIST_NODE *_next;
  Word *_word;
} WordList;

// Yay malloc went away
typedef struct SIMPLE_COMMAND {
  Word *_name;
  WordList *_args;
} SimpleCommand;

typedef struct LINK_LIST_SIMPLE_COMMAND {
  SimpleCommand *_command;
  struct LINK_LIST_SIMPLE_COMMAND *_next;
} SimpleCommandList;

typedef struct COMMAND {
  char *_outFile;
  char *_inputFile;
  int _append_input;
  SimpleCommandList *_simple_commands;
  int _background;
} CompoundCommand;

CompoundCommand *parser(char *line);

extern CompoundCommand *current_command;

extern SimpleCommand *current_simple_command;

int len(WordList *list);

void free_compound_command(CompoundCommand *command);

char *get_complete_command(Word *command, WordList *args);

char **generate_argv(Word *command, WordList *list, int starting_index);

extern WordList *current; // the current list word

extern WordList *nonopt; // start the execution from here

extern char *option_argument;

int get_command_opt(WordList *list, char *opts);

void reset_get_command_opt(void);

#endif //SHELDON_SRC_COMMAND_H
