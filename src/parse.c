//
// parse.c - implementation of functions such as tokenize input_string into argv, get _next SIMPLE_WORD into a string
// Created by kannav on 9/1/20.
//

#include "parse.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

int split_into_commands(char ***argv, char *inp) {
	char *token;
	token = strtok(inp, ";");
	int it = 0;
	int buffer_size = 10;
	*argv = (char **) malloc(buffer_size * sizeof(char *));

	while (token != NULL) {
		if (token == NULL) break;
		(*argv)[it++] = token;
		token = strtok(NULL, ";");
		if (it == buffer_size) {
			buffer_size += 10;
			*argv = (char **) realloc(*argv, buffer_size * sizeof(char *));
		}
	}

	return it;
}

enum parser_state {
  INIT,
  SIMPLE_WORD,
  SINGLE_QUOTE,
  DOUBLE_QUOTE,
  SPACE
};

static int curr_index = 0;
static int length = 0;
static char *input;
int is_quoted = 0;
static struct token *curr_token = (struct token *) NULL;

void load_token(struct token *tok, char *string, enum token_type state) {
	size_t len = strlen(string);
	tok->_text = (char *) malloc(len + 1);
	strncpy(tok->_text, string, len);
	tok->_text[len] = '\0';
	tok->_type = state;
}

void free_token(void) {
	if (curr_token != NULL) {
		if (curr_token->_text != NULL) {
			free(curr_token->_text);
		}
		free(curr_token);
	}
}

struct token *get_next_token(char *line) {
	enum parser_state state = INIT;
	char c;

	if (line != NULL) {
		curr_index = 0;
		input = line;
		length = (int) strlen(input) + 1;
	} else {
		if (curr_token != NULL) {
			if (curr_token->_text != NULL) {
				free(curr_token->_text);
			}
			free(curr_token);
		}
	}

	curr_token = (struct token *) malloc(sizeof(struct token));
	curr_token->_text = NULL;

	char *last_word = input + curr_index;
	if (curr_index == length) {
		return NULL;
	}

	while (curr_index < length) {
		c = input[curr_index];
		if (isspace(c)) {
			curr_token->_text = NULL;
			if (state != SINGLE_QUOTE && state != DOUBLE_QUOTE) {
				input[curr_index] = '\0';
				curr_index++;
				load_token(curr_token, last_word, STRING);
				if (strlen(curr_token->_text) > 0) {
					return curr_token;
				}
				continue;
			}
		} else if (c == '"') {
			if (state == DOUBLE_QUOTE) {
				input[curr_index] = '\0';
				curr_index++;
				load_token(curr_token, last_word, STRING);
				if (strlen(curr_token->_text) > 0) {
					return curr_token;
				}
				continue;
			} else if (state != SINGLE_QUOTE) {
				state = DOUBLE_QUOTE;
				last_word = input + curr_index + 1;
				input[curr_index] = '\0';
				is_quoted = 1;
			}
		} else if (c == '\'') {
			if (state == SINGLE_QUOTE) {
				input[curr_index] = '\0';
				curr_index++;
				load_token(curr_token, last_word, STRING);
				if (strlen(curr_token->_text) > 0) {
					return curr_token;
				}
				continue;
			} else if (state != DOUBLE_QUOTE) {
				state = SINGLE_QUOTE;
				last_word = input + curr_index + 1;
				input[curr_index] = '\0';
				is_quoted = 1;
			}
		} else {
			if (state != SINGLE_QUOTE && state != DOUBLE_QUOTE) {
				switch (c) {
					case '&': load_token(curr_token, "&", SYMBOL);
						input[curr_index] = '\0';
						curr_index++;
						if (strlen(curr_token->_text) > 0) {
							return curr_token;
						}
						continue;
					case '<': load_token(curr_token, "<", SYMBOL);
						input[curr_index] = '\0';
						curr_index++;
						if (strlen(curr_token->_text) > 0) {
							return curr_token;
						}
						continue;
					case '>':
						if (input[curr_index + 1] == '>') {
							load_token(curr_token, ">>", SYMBOL);
							input[curr_index] = input[curr_index + 1] = '\0';
							curr_index++;
							if (strlen(curr_token->_text) > 0) {
								return curr_token;
							}
							continue;
						} else {
							load_token(curr_token, ">", SYMBOL);
							input[curr_index] = '\0';
							curr_index++;
							if (strlen(curr_token->_text) > 0) {
								return curr_token;
							}
							continue;
						}
					case '|': load_token(curr_token, "|", SYMBOL);
						input[curr_index] = '\0';
						curr_index++;
						if (strlen(curr_token->_text) > 0) {
							return curr_token;
						}
						continue;
					case '\0': load_token(curr_token, last_word, STRING);
						curr_index++;
						if (strlen(curr_token->_text) > 0) {
							return curr_token;
						}
						continue;
					default:
						if (state == INIT) {
							state = SIMPLE_WORD;
							last_word = input + curr_index;
						}
						break;
				}
			}
		}
		curr_index++;
	}

	load_token(curr_token, last_word, STRING);
	if (strlen(curr_token->_text) > 0) {
		return curr_token;
	}
	return NULL;
}

