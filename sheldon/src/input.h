//
// Created by kannav on 9/1/20.
//

#ifndef SHELDON_SRC_INPUT_H
#define SHELDON_SRC_INPUT_H

#include <stdio.h>

typedef struct SHELDON_INPUT {
  char *input;
  size_t size;
  size_t buffer_size;
} __attribute__((aligned(32))) Input;

extern char *input;

extern size_t input_size;

extern size_t buffer_size;

char *read_input(void);

#endif //SHELDON_SRC_INPUT_H
