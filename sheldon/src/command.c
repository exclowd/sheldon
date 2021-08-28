//
// Created by kannav on 9/1/20.
//

#include "command.h"

#include <stdlib.h>
#include <string.h>

#include "parse.h"
#include "utils.h"

ccommand_t *current_command;

command_t *current_simple_command;

ccommand_t *new_compound_command() {
  ccommand_t *curr_comp;
  curr_comp = (ccommand_t *)malloc(sizeof(ccommand_t));
  curr_comp->_outFile = curr_comp->_inFile = NULL;
  curr_comp->_append_input = curr_comp->_background = 0;
  return curr_comp;
}

ccommand_t *generate_command(char *line) {
  /**
   * Generate command from the input string
   * @param line
   */
  ccommand_t *curr_comp = new_compound_command();
  int is_start = 1;
  int anticipate = 1;

  commandlist_t *head = (commandlist_t *)NULL;

  // keep inputting commands unless disturbed
  while (1) {
    token_t *token = get_next_token((is_start ? line : (char *)NULL));
    if (token == NULL) {
      if (anticipate) {
        EPRINTF("sheldon: syntax: command anticipated found nothing\n");
        return NULL;
      }
      break;
    }
    commandlist_t *cmnd_list =
        (commandlist_t *)malloc(sizeof(commandlist_t));
    command_t *curr_simp_cmnd = (command_t *)malloc(
        sizeof(command_t));  // current simple command
    curr_simp_cmnd->_args = NULL;
    // Put command in front of command list
    cmnd_list->_command = curr_simp_cmnd;
    cmnd_list->_next = NULL;

    /******* I do not understand this *******/
    if (!is_start) {
      if (curr_comp->_outFile != NULL) {
        free(curr_comp->_outFile);
        curr_comp->_outFile = NULL;
      }
    }

    if (token->_type == STRING) {
      size_t len = strlen(token->_text);
      curr_simp_cmnd->_name = (char *)malloc(len + 1);
      strncpy(curr_simp_cmnd->_name, token->_text, len);
      curr_simp_cmnd->_name[len] = '\0';
    } else {
      EPRINTF("sheldon: Anticipated command, found token %s\n", token->_text);
      return NULL;
    }

    arglist_t *args;

    int simple_command_complete = 0;

    anticipate = 0;

    // keep filling arguments unless interrupted
    while ((token = get_next_token(NULL)) != (token_t *)NULL) {
      arg_t *buf = (arg_t *)malloc(sizeof(arg_t));
      char *curr_word;
      char *text = token->_text;

      if (token->_type == STRING) {
        if (strlen(token->_text) > 0) {
          size_t len = strlen(token->_text);
          curr_word = (char *)malloc(len + 1);
          strncpy(curr_word, token->_text, len);
          curr_word[len] = '\0';
          buf->_text = curr_word;
        } else {
          free(buf);
          continue;
        }

        buf->_next = NULL;

        if (curr_simp_cmnd->_args == NULL) {
          /*connect _args to buf*/
          curr_simp_cmnd->_args = buf;
        } else {
          /*connect head to buf*/
          args->_next = buf;
        }
        // reposition head
        args = buf;
      } else {
        char c[2] = {*text, *(text + 1)};
        if (c[0] == '&') {
          curr_comp->_background = 1;
          simple_command_complete = 1;
        } else if (c[0] == '|') {
          simple_command_complete = 1;
          anticipate = 1;
        } else if (c[0] == '>') {
          if ((token = get_next_token(NULL)) != NULL &&
              token->_type == STRING) {
            size_t len = strlen(token->_text);
            if (curr_comp->_outFile != NULL) {
              EPRINTF("sheldon: syntax: multiple output sources found\n");
              return NULL;
            } else {
              curr_comp->_outFile = (char *)malloc(len + 1);
              strncpy(curr_comp->_outFile, token->_text, len);
              curr_comp->_outFile[len] = 0;
              printf("%s\n", c);
            }
          } else {
            EPRINTF("sheldon: syntax: expected filename after token %s\n",
                    text);
            return NULL;
          }
          int res = strncmp(c, ">>", 2);
          if (res == 0) {
            curr_comp->_append_input = 1;
          }
        } else if (c[0] == '<') {
          if (is_start) {
            if ((token = get_next_token(NULL)) != NULL &&
                token->_type == STRING) {
              size_t len = strlen(token->_text);
              if (curr_comp->_inFile != NULL) {
                EPRINTF("sheldon: syntax: multiple input sources detected\n");
                return NULL;
              } else {
                curr_comp->_inFile = (char *)malloc(len + 1);
                strncpy(curr_comp->_inFile, token->_text, len);
                curr_comp->_inFile[len] = 0;
              }
            } else {
              EPRINTF(
                  "sheldon: generate_command: expected filename after token "
                  "%s\n",
                  text);
              return NULL;
            }
          } else {
            EPRINTF("sheldon: syntax: multiple input sources detected\n");
            curr_comp->_inFile = NULL;
            return NULL;
          }
        } else {
          EPRINTF("token '%s' not recognized\n", text);
        }
        free(buf);
      }

      if (simple_command_complete) {
        break;
      }
    }

    if (is_start) {
      curr_comp->_simple_commands = cmnd_list;
      head = cmnd_list;
      is_start = 0;
    } else {
      head->_next = cmnd_list;
      head = cmnd_list;
    }
  }

  free_token();
  return curr_comp;
}

static void free_command(command_t *command) {
  arglist_t *head = command->_args;
  arglist_t *tmp;
  while (head != NULL) {
    tmp = head;
    head = head->_next;
    free(tmp->_text);
    free(tmp);
  }
  free(command->_name);
  free(command);
}

void free_compound_command(ccommand_t *cc) {
  commandlist_t *head = cc->_simple_commands;
  commandlist_t *tmp;

  while (head != NULL) {
    tmp = head;
    head = head->_next;
    free_command(tmp->_command);
    free(tmp);
  }
  free(cc->_outFile);
  free(cc->_inFile);
  free(cc);
}

int len(arglist_t *list) {
  register int i;
  if (list == (arglist_t *)NULL) {
    return 0;
  }
  for (i = 0; list; list = list->_next, i++)
    ;
  return i;
}

/* getting alternate representations of the command*/

char *get_complete_command(char *command, arglist_t *args) {
  char *str;
  size_t len = strlen(command);
  for (arg_t *curr = args; curr != NULL; curr = curr->_next) {
    len += 1 + strlen(curr->_text);
  }
  str = (char *)malloc(len + 1);
  size_t pos = 0;
  strncpy(str + pos, command, strlen(command));
  pos += strlen(command);
  for (arglist_t *curr = args; curr != NULL; curr = curr->_next) {
    str[pos++] = ' ';
    strncpy(str + pos, curr->_text, strlen(curr->_text));
    pos += strlen(curr->_text);
  }
  str[len] = '\0';
  return str;
}

char **generate_argv(char *command, arglist_t *list, int starting_index) {
  int count;
  char **array;

  count = len(list);
  array = (char **)calloc((2 + count + starting_index), sizeof(char *));
  array[0] = command;

  for (count = starting_index + 1; list; count++, list = list->_next) {
    array[count] = list->_text;
  }
  array[count] = (char *)NULL;

  return (array);
}

/* utilities for  getting command options */

static int idx = 1;

static arglist_t *head = (arglist_t *)NULL;  // saves last used list

arglist_t *current = (arglist_t *)NULL;  // the current list word

arglist_t *nonopt;  // start the execution from here

int get_command_opt(arglist_t *list, char *opts) {
  char c;

  if (list == 0) {
    nonopt = NULL;
    return -1;  // Terminate no arguments
  }

  if (list != head || head == NULL) {
    idx = 1;
    current = head = list;
  }

  if (idx == 1) {  // starting a arg
    if (current == NULL) {
      // nothing here
      head = (arglist_t *)NULL;
      if (nonopt == NULL) {
        nonopt = current;  // If it is null then the standard measures are to be
                           // taken
      }
      return -1;  // Terminate
    }
    if (*(current->_text) != '-') {
      if (nonopt == NULL) {
        nonopt = current;
      }
      current = current->_next;
      return 0;
    }
  }

  c = current->_text[idx];

  if (strchr(opts, c) == NULL) {
    nonopt = current;

    return '?';  // None of the options match not a valid arg
  }

  if (current->_text[++idx] == '\0') {
    current = current->_next;
    idx = 1;
  }

  return c;
}

void reset_get_command_opt(void) { head = current = nonopt = (arglist_t *)NULL; }
