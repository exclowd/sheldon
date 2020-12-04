//
// Created by kannav on 9/1/20.
//

#include <stdlib.h>
#include <string.h>
#include "parse.h"
#include "utils.h"
#include "command.h"

CompoundCommand *current_command;

SimpleCommand *current_simple_command;

CompoundCommand *parser(char *line) {
  CompoundCommand *curr_comp;

  curr_comp = (CompoundCommand *) malloc(sizeof(CompoundCommand));

  curr_comp->_outFile = curr_comp->_inputFile = NULL;
  curr_comp->_append_input = 0;
  curr_comp->_background = 0;

  register int first_command = 1;
  int anticipate_command = 1;

  SimpleCommandList *head = (SimpleCommandList *) NULL;

  // keep inputting commands unless disturbed
  while (1) {
	struct token *token = NULL;
	if (first_command) {
	  token = get_next_token(line);
	} else {
	  token = get_next_token(NULL);
	}
	if (token == NULL) {
	  if (anticipate_command) {
		EPRINTF("sheldon: syntax: command anticipated found nothing\n");
		return NULL;
	  } else {
		break;
	  }
	}

	SimpleCommandList *command_node = (SimpleCommandList *) malloc(sizeof(SimpleCommandList));
	SimpleCommand *curr = (SimpleCommand *) malloc(sizeof(SimpleCommand)); // current simple command
	command_node->_command = curr;
	command_node->_next = NULL;

	curr->_name = (Word *) malloc(sizeof(Word));
	curr->_args = NULL;

	if (first_command == 0) {
	  if (curr_comp->_outFile != NULL) {
		free(curr_comp->_outFile);
		curr_comp->_outFile = NULL;
	  }
	}

	if (token->_text == NULL) {
	  EPRINTF("sheldon: Anticipated command, found nothing\n");
	  return NULL;
	}

	if (token->_type == STRING) {
	  size_t len = strlen(token->_text);
	  curr->_name->_text = (char *) malloc(len + 1);
	  strncpy(curr->_name->_text, token->_text, len);
	  curr->_name->_text[len] = '\0';
	} else {
	  EPRINTF("sheldon: Anticipated command, found token %s\n", token->_text);
	  return NULL;
	}

	WordList *args;

	int simple_command_complete = 0;

	anticipate_command = 0;

	// keep filling arguments unless interrupted
	while ((token = get_next_token(NULL)) != (struct token *) NULL) {

	  WordList *buf = (WordList *) malloc(sizeof(WordList));
	  Word *curr_word = (Word *) malloc(sizeof(Word));

	  char *text = token->_text;

	  if (token->_type == STRING) {

		if (strlen(token->_text) > 0) {
		  buf->_word = curr_word;
		  size_t len = strlen(token->_text);
		  curr_word->_text = (char *) malloc(len + 1);
		  strncpy(curr_word->_text, token->_text, len);
		  curr_word->_text[len] = '\0';
		} else {
		  free(curr_word);
		  free(buf);
		  continue;
		}

		buf->_next = NULL;

		if (curr->_args == NULL) {
		  /*connect _args to buf*/
		  curr->_args = buf;
		} else {
		  /*connect head to buf*/
		  args->_next = buf;
		}

		// reposition head
		args = buf;
	  } else {
		char c[2] = {*text, *(text + 1)};
		switch (c[0]) {
		case '&':curr_comp->_background = 1;
		  simple_command_complete = 1;
		  break;
		case '|':simple_command_complete = 1;
		  anticipate_command = 1;
		  break;
		case '>':
		  if ((token = get_next_token(NULL)) != NULL && token->_type == STRING) {
			size_t len = strlen(token->_text);
			if (curr_comp->_outFile != NULL) {
			  EPRINTF("sheldon: syntax: multiple output sources found\n");
			  return NULL;
			} else {
			  curr_comp->_outFile = (char *) malloc(len + 1);
			  strncpy(curr_comp->_outFile, token->_text, len);
			  curr_comp->_outFile[len] = 0;
			}
		  } else {
			EPRINTF("sheldon: syntax: expected filename after token %s\n", text);
			return NULL;
		  }
		  if (strncmp(c, ">>", 2) == 0) {
			curr_comp->_append_input = 1;
		  }

		  break;
		case '<':
		  if (first_command) {
			if ((token = get_next_token(NULL)) != NULL && token->_type == STRING) {
			  size_t len = strlen(token->_text);
			  if (curr_comp->_inputFile != NULL) {
				EPRINTF("sheldon: syntax: multiple input sources detected\n");
				return NULL;
			  } else {
				curr_comp->_inputFile = (char *) malloc(len + 1);
				strncpy(curr_comp->_inputFile, token->_text, len);
				curr_comp->_inputFile[len] = 0;
			  }
			} else {
			  EPRINTF("sheldon: parser: expected filename after token %s\n", text);
			  return NULL;
			}
		  } else {
			EPRINTF("sheldon: syntax: multiple input sources detected\n");
			curr_comp->_inputFile = NULL;
			return NULL;
		  }
		  break;
		default:EPRINTF("token %s not recongnized\n", text);
		}
		free(curr_word);
		free(buf);
	  }

	  if (simple_command_complete) {
		break;
	  }
	}

	if (first_command) {
	  curr_comp->_simple_commands = command_node;
	  head = command_node;
	  first_command = 0;
	} else {
	  head->_next = command_node;
	  head = command_node;
	}
  }

  free_token();
  return curr_comp;
}

static void free_command(SimpleCommand *command) {
  WordList *head = command->_args;
  WordList *tmp;

  while (head != NULL) {
	tmp = head;
	head = head->_next;
	free(tmp->_word);
	free(tmp);
  }

  free(command->_name->_text);
  free(command->_name);
  free(command);
}

void free_compound_command(CompoundCommand *cc) {
  SimpleCommandList *head = cc->_simple_commands;
  SimpleCommandList *tmp;

  while (head != NULL) {
	tmp = head;
	head = head->_next;
	free_command(tmp->_command);
	free(tmp);
  }
  free(cc->_outFile);
  free(cc->_inputFile);
  free(cc);
}

int len(WordList *list) {
  register int i;
  if (list == (WordList *) NULL) {
	return 0;
  }
  for (i = 0; list; list = list->_next, i++);
  return i;
}

/* getting alternate representations of the command*/

char *get_complete_command(Word *command, WordList *args) {
  char *str;
  size_t len = strlen(command->_text);
  for (WordList *curr = args; curr != NULL; curr = curr->_next) {
	len += 1 + strlen(curr->_word->_text);
  }
  str = (char *) malloc(len + 1);
  size_t pos = 0;
  strncpy(str + pos, command->_text, strlen(command->_text));
  pos += strlen(command->_text);
  for (WordList *curr = args; curr != NULL; curr = curr->_next) {
	str[pos++] = ' ';
	strncpy(str + pos, curr->_word->_text, strlen(curr->_word->_text));
	pos += strlen(curr->_word->_text);
  }
  str[len] = '\0';
  return str;
}

char **generate_argv(Word *command, WordList *list, int starting_index) {
  int count;
  char **array;

  count = len(list);
  array = (char **) malloc((2 + count + starting_index) * sizeof(char *));

  array[0] = command->_text;

  for (count = 1; count <= starting_index; count++) {
	array[count] = (char *) NULL;
  }
  for (count = starting_index + 1; list; count++, list = list->_next) {
	array[count] = list->_word->_text;
  }
  array[count] = (char *) NULL;

  return (array);
}

/* utilities for  getting command options */

static int idx = 1;

static WordList *head = (WordList *) NULL; // saves last used list

WordList *current = (WordList *) NULL; // the current list word

WordList *nonopt; // start the execution from here

int get_command_opt(WordList *list, char *opts) {
  char c;

  if (list == 0) {
	nonopt = NULL;
	return -1; // Terminate no arguments
  }

  if (list != head || head == NULL) {
	idx = 1;
	current = head = list;
  }

  if (idx == 1) { // starting a arg
	if (current == NULL) {
	  // nothing here
	  head = (WordList *) NULL;
	  if (nonopt == NULL) {
		nonopt = current; // If it is null then the standard measures are to be taken
	  }
	  return -1; // Terminate
	}
//        printf("current->SIMPLE_WORD->_text %s\n", current->SIMPLE_WORD->_text);
	if (*(current->_word->_text) != '-') {
	  if (nonopt == NULL) {
		nonopt = current;
	  }
	  current = current->_next;
	  return 0;
	}
  }

  c = current->_word->_text[idx];

  if (strchr(opts, c) == NULL) {

	nonopt = current;

	return '?'; // None of the options match not a valid arg
  }

  if (current->_word->_text[++idx] == '\0') {
	current = current->_next;
	idx = 1;
  }

  return c;
}

void reset_get_command_opt(void) {
  head = current = nonopt = (WordList *) NULL;
}
