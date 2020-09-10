//
// Created by kannav on 9/1/20.
//

#ifndef SHELDON_PARSE_H
#define SHELDON_PARSE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>

void display_prompt (void);

int tokenize_input (char *** argv, char * input);

char * get_next_word (char * line);

extern int is_quoted;

#endif //SHELDON_PARSE_H
