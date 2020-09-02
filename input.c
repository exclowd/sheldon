//
// Created by kannav on 9/1/20.
//

#include "input.h"
#include "utils.h"

enum input_state {
    INIT,
    WORD,
    SINGLE_QUOTE,
    DOUBLE_QUOTE,
    SPACE
};

static char *input;

size_t buffer_size = 100;

size_t input_size;

void expand_tilda_to_home(void) {
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
    enum input_state state = INIT;
    buffer_size = 100;
    input = (char *) malloc(buffer_size);
    memset(input, 0, buffer_size);
    register char c;
    input_size = 0;
    while ((c = getchar())) {
        if (c == EOF) {
            free(input);
            exit_safely();
        }
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
                if (state != SINGLE_QUOTE && state != DOUBLE_QUOTE) {
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
            if (state != SINGLE_QUOTE && state != DOUBLE_QUOTE) {
                state = INIT;
            }
        } else if (c == '~') {
            if (state != SINGLE_QUOTE && state != DOUBLE_QUOTE) {
                expand_tilda_to_home();
                state=WORD;
                continue;
            } else {
                input[input_size] = c;
            }
        } else {
            input[input_size] = c;
            if (state != SINGLE_QUOTE && state != DOUBLE_QUOTE) {
                state = WORD;
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
