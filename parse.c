//
// Created by kannav on 9/1/20.
//

#include "parse.h"

int tokenize_input(char ***argv, char *inp) {
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

int i = 0;

static int len = 0;

static char *input;

int is_quoted = 0;

char *get_next_word(char *line) {
    enum parser_state state = INIT;
    char c;

    if (line != NULL) {
        i = 0;
        input = line;
        len = (int) strlen(input);
    }

    char *last_word;
    if (i == len) {
        return NULL;
    }

    is_quoted = 0;

    while (i < len) {
        c = input[i];
        if (isspace(c)) {

            if (state != SINGLE_QUOTE && state != DOUBLE_QUOTE) {
                input[i] = '\0';
                i++;
                return last_word;
            }

        } else if (c == '"') {

            if (state == DOUBLE_QUOTE) {
                input[i] = '\0';
                i++;
                return last_word;
            } else if (state != SINGLE_QUOTE) {
                state = DOUBLE_QUOTE;
                last_word = input + i + 1;
                input[i] = '\0';
                is_quoted = 1;
            }

        } else if (c == '\'') {

            if (state == SINGLE_QUOTE) {
                input[i] = '\0';
                i++;
                return last_word;
            } else if (state != DOUBLE_QUOTE) {
                state = SINGLE_QUOTE;
                last_word = input + i + 1;
                input[i] = '\0';
                is_quoted = 1;
            }

        } else {

            if (state == INIT) {
                last_word = input + i;
                state = WORD;
            }

        }
        i++;
    }
    return last_word;
}

