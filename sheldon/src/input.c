//
// Created by kannav on 9/1/20.
//

#include "input.h"
#include "utils.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>

/*known aliases*/
#define IS_QUOTE(state) ((state) == SINGLE_QUOTE || (state) == DOUBLE_QUOTE)

enum InputState {
  INIT,
  SIMPLE_WORD,
  SINGLE_QUOTE,
  DOUBLE_QUOTE,
  SPACE
};

char *input;

static int escaped = 0;

size_t buffer_size = 100;
size_t input_size;

static void expand_tilda_to_home(void) {
  size_t len = strlen(home);
  for (int i = 0; i < len; i++) {
	input[input_size++] = home[i];
	if (input_size == buffer_size - 1) {
	  buffer_size += 100;
	}
	input = (char *) realloc(input, buffer_size);
  }
}

char *read_input() {
  enum InputState state = INIT;
  buffer_size = 100;
  input = (char *) malloc(buffer_size);
  memset(input, 0, buffer_size);
  char c;
  input_size = 0;
  tcsetpgrp(shell_terminal, shell_pgid);
  while ((c = (char) getchar())) {
	if (c == EOF) {
	  free(input);
	  exit_safely(0);
	}
	int skip = 0;
	if (escaped) {
	  escaped = 0;
	  if (IS_QUOTE(state)) {
		switch (c) {
		case 'n': input[input_size] = '\n';
		  skip = 1;
		  break;
		case 't': input[input_size] = '\t';
		  skip = 1;
		  break;
		case 's': input[input_size] = ' ';
		  skip = 1;
		  break;
		case '\\': input[input_size] = '\\';
		  skip = 1;
		  break;
		default: break;
		}
	  } else {
		EPRINTF("sheldon: input: stray escape sequence found, quote it first\n");
		return NULL;
	  }
	}

	if (!skip) {
	  if (isspace(c)) {
		if (c == '\n') {
		  if (state == SINGLE_QUOTE) {
			input[input_size] = c;
			printf("squote>");
		  } else if (state == DOUBLE_QUOTE) {
			input[input_size] = c;
			printf("dquote>");
		  } else {
			break;
		  }
		}
		if (state == SPACE || state == INIT) {
		  continue;
		} else {
		  input[input_size] = c;
		  if (!IS_QUOTE(state)) {
			state = SPACE;
		  }
		}
	  } else if (c == '"') {
		input[input_size] = c;
		if (state == DOUBLE_QUOTE) {
		  state = INIT;
		} else if (state != SINGLE_QUOTE) {
		  state = DOUBLE_QUOTE;
		}
	  } else if (c == '\'') {
		input[input_size] = c;
		if (state == SINGLE_QUOTE) {
		  state = INIT;
		} else if (state != DOUBLE_QUOTE) {
		  state = SINGLE_QUOTE;
		}
	  } else if (c == ';') {
		input[input_size] = c;
		if (!IS_QUOTE(state)) {
		  state = INIT;
		}
	  } else if (c == '~') {
		if (!IS_QUOTE(state)) {
		  expand_tilda_to_home();
		  state = SIMPLE_WORD;
		  continue;
		} else {
		  input[input_size] = c;
		}
	  } else if (c == '\\') {
		escaped = 1;
		continue;
	  } else {
		input[input_size] = c;
		if (!IS_QUOTE(state)) {
		  state = SIMPLE_WORD;
		}
	  }
	}
	input_size++;
	if (input_size == buffer_size - 1) {
	  buffer_size += 100;
	}
	input = (char *) realloc(input, buffer_size);
  }
  input[input_size] = '\0';

  return input;
}
