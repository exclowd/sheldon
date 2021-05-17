//
// Created by kannav on 9/1/20.
//

#ifndef SHELDON_SRC_PARSE_H
#define SHELDON_SRC_PARSE_H

enum TokenType { SYMBOL, STRING };

struct token {
  enum TokenType _type;
  char *_text;
} __attribute__((aligned(16)));

void display_prompt(void);

int split_into_commands(char ***argv, char *input);

struct token *get_next_token(char *line);

void free_token(void);

#endif  // SHELDON_SRC_PARSE_H
