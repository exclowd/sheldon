//
// Created by kannav on 9/1/20.
//

#ifndef SHELDON_SRC_COMMAND_H
#define SHELDON_SRC_COMMAND_H

typedef struct COMMAND_NODE {
  char *_text;
  struct COMMAND_NODE *_next;
} __attribute__((aligned(16))) arg_t;

typedef arg_t arglist_t;

// Yay malloc went away
typedef struct COMMAND {
  char *_name;
  arglist_t *_args;
} __attribute__((aligned(16))) command_t;

typedef struct COMMAND_LIST {
  command_t *_command;
  struct COMMAND_LIST *_next;
} __attribute__((aligned(16))) commandlist_t;

typedef struct COMPOUND_COMMAND {
  char *_outFile;
  char *_inFile;
  int _append_input;
  commandlist_t *_simple_commands;
  int _background;
} __attribute__((aligned(32))) __attribute__((packed)) ccommand_t;

extern ccommand_t *generate_command(char *line);

extern ccommand_t *current_command;

extern command_t *current_simple_command;

extern int len(arglist_t *list);

extern void free_compound_command(ccommand_t *command);

extern char *get_complete_command(char *command, arglist_t *args);

extern char **generate_argv(char *command, arglist_t *list, int starting_index);

extern arglist_t *current;  // the current list word

extern arglist_t *nonopt;  // start the execution from here

extern int get_command_opt(arglist_t *list, char *opts);

extern void reset_get_command_opt(void);

#endif  // SHELDON_SRC_COMMAND_H
