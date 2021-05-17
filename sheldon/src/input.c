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
#define IS_QUOTE(state) ((state) == SQUOTE || (state) == DQUOTE)

enum InputState {
  INIT,
  WORD,
  SQUOTE,
  DQUOTE,
  SPACE
};

char *input;

static int escaped = 0;

size_t buffer_size = 100;
size_t inp_it;

static void expand_tilda_to_home(void) {
  size_t len = strlen(home);
  for (int i = 0; i < len; i++) {
	input[inp_it++] = home[i];
	if (inp_it == buffer_size - 1) {
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
  inp_it = 0;
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
		case 'n': input[inp_it] = '\n';
		  skip = 1;
		  break;
		case 't': input[inp_it] = '\t';
		  skip = 1;
		  break;
		case 's': input[inp_it] = ' ';
		  skip = 1;
		  break;
		case '\\': input[inp_it] = '\\';
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
		  if (state == SQUOTE) {
			input[inp_it] = c;
			printf("squote>");
		  } else if (state == DQUOTE) {
			input[inp_it] = c;
			printf("dquote>");
		  } else {
			break;
		  }
		}
		if (state == SPACE || state == INIT) {
		  continue;
		} else {
		  input[inp_it] = c;
		  if (!IS_QUOTE(state)) {
			state = SPACE;
		  }
		}
	  } else if (c == '"') {
		input[inp_it] = c;
		if (state == DQUOTE) {
		  state = INIT;
		} else if (state != SQUOTE) {
		  state = DQUOTE;
		}
	  } else if (c == '\'') {
		input[inp_it] = c;
		if (state == SQUOTE) {
		  state = INIT;
		} else if (state != DQUOTE) {
		  state = SQUOTE;
		}
	  } else if (c == ';') {
		input[inp_it] = c;
		if (!IS_QUOTE(state)) {
		  state = INIT;
		}
	  } else if (c == '~') {
		if (!IS_QUOTE(state)) {
		  expand_tilda_to_home();
		  state = WORD;
		  continue;
		} else {
		  input[inp_it] = c;
		}
	  } else if (c == '\\') {
		escaped = 1;
		continue;
	  } else {
		input[inp_it] = c;
		if (!IS_QUOTE(state)) {
		  state = WORD;
		}
	  }
	}
	inp_it++;
	if (inp_it == buffer_size - 1) {
	  buffer_size += 100;
	}
	input = (char *) realloc(input, buffer_size);
  }
  input[inp_it] = '\0';

  return input;
}
