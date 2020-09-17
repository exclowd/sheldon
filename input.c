//
// Created by kannav on 9/1/20.
//

#include "input.h"
#include "utils.h"


#define IS_QUOTE(state) ((state) == SINGLE_QUOTE || (state) == DOUBLE_QUOTE)

enum input_state {
  INIT,
  WORD,
  SINGLE_QUOTE,
  DOUBLE_QUOTE,
  SPACE
};

static char *input_string;

size_t buffer_size = 100;
size_t input_size;

void expand_tilda_to_home(void) {
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
                state = WORD;
                continue;
            } else {
                input_string[input_size] = c;
            }
        } else {
            input_string[input_size] = c;
            if (!IS_QUOTE(state)) {
                state = WORD;
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
