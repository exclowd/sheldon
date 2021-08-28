//
// parse.c - implementation of functions such as tokenize input_string into
// argv, get _next SIMPLE_WORD into a string Created by kannav on 9/1/20.
//

#include "parse.h"

#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#define NULL_TERMINATE(x, y) x[(y)++] = '\0'

// argv is pointer ot a 2-D array
int split_into_commands(char ***argv, char *inp) {
  register int i = 0;
  int buffer_size = 10;
  *argv = (char **)malloc(buffer_size * sizeof(char *));

  for (char *token = strtok(inp, ";"); token != NULL;
       token = strtok(NULL, ";")) {
    (*argv)[i++] = token;
    if (i == buffer_size) {
      buffer_size += 10;
      *argv = (char **)realloc(*argv, buffer_size * sizeof(char *));
    }
  }
  return i;
}

enum ParserState { INIT, WORD, SQUOTE, DQUOTE, SPACE };

static int i = 0;  // number of parsed tokens in current command line
static int l = 0;
static char *input;  // The input source
static token_t *curr_token = (token_t *)NULL;

static void load_token(token_t *tok, char *string, enum TOKEN_TYPE state) {
  size_t len = strlen(string);
  tok->_text = (char *)malloc(len + 1);
  strncpy(tok->_text, string, len);
  tok->_text[len] = '\0';
  tok->_type = state;
}

token_t *get_next_token(char *line) {
  enum ParserState curr_state = INIT;
  char c;

  if (line != NULL) {  // beginning a new command
    i = 0;
    input = line;
    l = (int)strlen(input) + 1;
  } else {
    if (curr_token != NULL) {
      if (curr_token->_text != NULL) {
        free(curr_token->_text);
      }
      free(curr_token);
    }
  }

  curr_token = (token_t *)malloc(sizeof(token_t));
  curr_token->_text = NULL;

  char *starting_pos = input + i;
  if (i == l) {
    return NULL;
  }

  while (i < l) {
    c = input[i];
    if (isspace(c)) {
      if (curr_state != SQUOTE && curr_state != DQUOTE) {
        NULL_TERMINATE(input, i);
        if (strlen(starting_pos) > 0) {
          load_token(curr_token, starting_pos, STRING);
          return curr_token;
        }
      }
    } else if (c == '"') {
      if (curr_state == DQUOTE) {
        NULL_TERMINATE(input, i);
        if (strlen(starting_pos) > 0) {
          load_token(curr_token, starting_pos, STRING);
          return curr_token;
        }
      } else if (curr_state != SQUOTE) {
        curr_state = DQUOTE;
        starting_pos = input + i + 1;
        input[i] = '\0';
      }
    } else if (c == '\'') {
      if (curr_state == SQUOTE) {
        NULL_TERMINATE(input, i);
        if (strlen(starting_pos) > 0) {
          load_token(curr_token, starting_pos, STRING);
          return curr_token;
        }
      } else if (curr_state != DQUOTE) {
        curr_state = SQUOTE;
        starting_pos = input + i + 1;
        input[i] = '\0';
      }
    } else if (curr_state != SQUOTE && curr_state != DQUOTE) {
      char str[2] = "\0";
      switch (c) {
        case '&':
        case '<':
        case '|':
          str[0] = c;
          load_token(curr_token, str, SYMBOL);
          NULL_TERMINATE(input, i);
          return curr_token;
        case '>':
          if (input[i + 1] == '>') {
            load_token(curr_token, ">>", SYMBOL);
            NULL_TERMINATE(input, i);
            return curr_token;
          } else {
            load_token(curr_token, ">", SYMBOL);
            NULL_TERMINATE(input, i);
            return curr_token;
          }
        case '\0':
          load_token(curr_token, starting_pos, STRING);
          NULL_TERMINATE(input, i);
          return curr_token;
        default:
          if (curr_state == INIT) {
            curr_state = WORD;
            starting_pos = input + i;
          }
          i++;
          break;
      }
    } else {
      i++;
    }
  }

  load_token(curr_token, starting_pos, STRING);
  if (strlen(curr_token->_text) > 0) {
    return curr_token;
  }
  return NULL;
}


void free_token(void) {
  if (curr_token != NULL) {
    if (curr_token->_text != NULL) {
      free(curr_token->_text);
    }
    free(curr_token);
  }
}