//
// Created by kannav on 9/1/20.
//

#include "command.h"
#include "parse.h"
#include "utils.h"

compound_command *current_command;

compound_command *Parser(char *line) {
	compound_command *curr_comp;

	curr_comp = (compound_command *) malloc(sizeof(compound_command));

	curr_comp->_outFile = curr_comp->_inputFile = NULL;
	curr_comp->_append_input = 0;
	curr_comp->_background = 0;

	register int first_command = 1;
	int anticipate_command = 1;

	simple_command_list *head = (simple_command_list *) NULL;

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
				eprintf("sheldon: syntax: command anticipated found nothing");
				return NULL;
			} else {
				break;
			}
		}

		simple_command_list *command_node = (simple_command_list *) malloc(sizeof(simple_command_list));
		simple_command *curr = (simple_command *) malloc(sizeof(simple_command)); // current simple command
		command_node->_command = curr;
		command_node->_next = NULL;

		curr->_name = (word *) malloc(sizeof(word));
		curr->_args = NULL;

		if (first_command == 0) {
			if (curr_comp->_outFile != NULL) {
				free(curr_comp->_outFile);
				curr_comp->_outFile = NULL;
			}
		}

		if (token->_text == NULL) {
			eprintf("sheldon: Anticipated command, found nothing");
			return NULL;
		}

		if (token->_type == STRING) {
			size_t len = strlen(token->_text);
			curr->_name->_text = (char *) malloc(len + 1);
			strncpy(curr->_name->_text, token->_text, len);
			curr->_name->_text[len] = '\0';
		} else {
			eprintf("sheldon: Anticipated command, found token %s", token->_text);
			return NULL;
		}

		word_list *args;

		int simple_command_complete = 0;

		anticipate_command = 0;

		// keep filling arguments unless interrupted
		while ((token = get_next_token(NULL)) != (struct token *) NULL) {

			word_list *buf = (word_list *) malloc(sizeof(word_list));
			word *curr_word = (word *) malloc(sizeof(word));

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
					// connect _args to buf
					curr->_args = buf;
				} else {
					// connect head to buf
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
								eprintf("sheldon: syntax: multiple output sources found");
								return NULL;
							} else {
								curr_comp->_outFile = (char *) malloc(len + 1);
								strncpy(curr_comp->_outFile, token->_text, len);
								curr_comp->_outFile[len] = 0;
							}
						} else {
							eprintf("sheldon: syntax: expected filename after token %s", text);
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
									eprintf("sheldon: syntax: multiple input sources detected");
									return NULL;
								} else {
									curr_comp->_inputFile = (char *) malloc(len + 1);
									strncpy(curr_comp->_inputFile, token->_text, len);
									curr_comp->_inputFile[len] = 0;
								}
							} else {
								eprintf("sheldon: parser: expected filename after token %s", text);
								return NULL;
							}
						} else {
							if (curr_comp->_inputFile != NULL) {
								eprintf("sheldon: syntax: multiple input sources detected");
								free(curr_comp->_inputFile);
								curr_comp->_inputFile = NULL;
								return NULL;
							}
						}
						break;
					default:eprintf("token %s not recongnized", text);
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

	return curr_comp;
}

void free_command(simple_command *command) {
	word_list *head = command->_args;
	word_list *tmp;

	while (head != NULL) {
		tmp = head;
		head = head->_next;
		free(tmp->_word);
		free(tmp);
	}

	free(command->_name);
	free(command);
}

int list_length(word_list *list) {
	register int i;
	for (i = 0; list; list = list->_next, i++);
	return i;
}

// always remember to free this pointer

char **generate_argv(word *command, word_list *list, int starting_index) {
	int count;
	char **array;

	count = list_length(list);
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

static int idx = 1;

static word_list *head = (word_list *) NULL; // saves last used list

word_list *current = (word_list *) NULL; // the current list word

word_list *nonopt; // start the execution from here

int get_command_opt(word_list *list, char *opts) {
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
			head = (word_list *) NULL;
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
	head = current = nonopt = (word_list *) NULL;
}
