//
// Created by kannav on 9/1/20.
//

#include "input.h"
#include "utils.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

/*known aliases*/
#define IS_QUOTE(state) ((state) == SINGLE_QUOTE || (state) == DOUBLE_QUOTE)

enum input_state {
  INIT,
  SIMPLE_WORD,
  SINGLE_QUOTE,
  DOUBLE_QUOTE,
  SPACE
};

char *input_string;

static int escaped = 0;

size_t buffer_size = 100;
size_t input_size;

static void expand_tilda_to_home(void) {
  size_t len = strlen(home);
  for (int i = 0; i < len; i++) {
	input_string[input_size++] = home[i];
	if (input_size == buffer_size - 1) {
	  buffer_size += 100;
	}
	input_string = (char *) realloc(input_string, buffer_size);
  }
}

char *read_input() {
  enum input_state state = INIT;
  buffer_size = 100;
  input_string = (char *) malloc(buffer_size);
  memset(input_string, 0, buffer_size);
  register char c;
  input_size = 0;
  while ((c = (char) getchar())) {
	if (c == EOF) {
	  free(input_string);
	  exit_safely(0);
	}
	int skip = 0;
	if (escaped) {
	  escaped = 0;
	  if (IS_QUOTE(state)) {
		switch (c) {
		case 'n': input_string[input_size] = '\n';
		  skip = 1;
		  break;
		case 't': input_string[input_size] = '\t';
		  skip = 1;
		  break;
		case 's': input_string[input_size] = ' ';
		  skip = 1;
		  break;
		case '\\': input_string[input_size] = '\\';
		  skip = 1;
		  break;
		default:;
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
			input_string[input_size] = c;
			printf("squote>");
		  } else if (state == DOUBLE_QUOTE) {
			input_string[input_size] = c;
			printf("dquote>");
		  } else {
			break;
		  }
		}
		if (state == SPACE || state == INIT) {
		  continue;
		} else {
		  input_string[input_size] = c;
		  if (!IS_QUOTE(state)) {
			state = SPACE;
		  }
		}
	  } else if (c == '"') {
		input_string[input_size] = c;
		if (state == DOUBLE_QUOTE) {
		  state = INIT;
		} else if (state != SINGLE_QUOTE) {
		  state = DOUBLE_QUOTE;
		}
	  } else if (c == '\'') {
		input_string[input_size] = c;
		if (state == SINGLE_QUOTE) {
		  state = INIT;
		} else if (state != DOUBLE_QUOTE) {
		  state = SINGLE_QUOTE;
		}
	  } else if (c == ';') {
		input_string[input_size] = c;
		if (!IS_QUOTE(state)) {
		  state = INIT;
		}
	  } else if (c == '~') {
		if (!IS_QUOTE(state)) {
		  expand_tilda_to_home();
		  state = SIMPLE_WORD;
		  continue;
		} else {
		  input_string[input_size] = c;
		}
	  } else if (c == '\\') {
		escaped = 1;
		continue;
	  } else {
		input_string[input_size] = c;
		if (!IS_QUOTE(state)) {
		  state = SIMPLE_WORD;
		}
	  }
	}
	input_size++;
	if (input_size == buffer_size - 1) {
	  buffer_size += 100;
	}
	input_string = (char *) realloc(input_string, buffer_size);
  }
  input_string[input_size] = '\0';

  return input_string;
}
