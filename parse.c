//
// parse.c - implementation of functions such as tokenize input_string into argv, get next word into a string
// Created by kannav on 9/1/20.
//

#include "parse.h"

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
    WORD,
    SINGLE_QUOTE,
    DOUBLE_QUOTE,
    SPACE
};

static int curr_index = 0;
static int length = 0;
static char *input;
int is_quoted;

char *get_next_word(char *line) {
    enum parser_state state = INIT;
    char c;

    if (line != NULL) {
        curr_index = 0;
        input = line;
        length = (int) strlen(input);
    }

    char *last_word;
    if (curr_index == length) {
        return NULL;
    }

    is_quoted = 0;

    while (curr_index < length) {
        c = input[curr_index];
        if (isspace(c)) {
            if (state != SINGLE_QUOTE && state != DOUBLE_QUOTE) {
                input[curr_index] = '\0';
                curr_index++;
                return last_word;
            }
        } else if (c == '"') {
            if (state == DOUBLE_QUOTE) {
                input[curr_index] = '\0';
                curr_index++;
                return last_word;
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
                return last_word;
            } else if (state != DOUBLE_QUOTE) {
                state = SINGLE_QUOTE;
                last_word = input + curr_index + 1;
                input[curr_index] = '\0';
                is_quoted = 1;
            }
        } else {
            if (state == INIT) {
                last_word = input + curr_index;
                state = WORD;
            }
        }
        curr_index++;
    }

    return last_word;
}

