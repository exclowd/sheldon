//
// Created by kannav on 9/1/20.
//

#ifndef SHELDON_SRC_PARSE_H
#define SHELDON_SRC_PARSE_H

enum TOKEN_TYPE { SYMBOL, STRING };

typedef struct TOKEN {
  enum TOKEN_TYPE _type;
  char *_text;
} __attribute__((aligned(16))) token_t;

int split_into_commands(char ***argv, char *input);

token_t *get_next_token(char *line);

void free_token(void);

#endif  // SHELDON_SRC_PARSE_H
