//
// Created by kannav on 9/25/20.
//

#ifndef SHELDON_BUILTINS_ENV_H
#define SHELDON_BUILTINS_ENV_H
#include "../src/command.h"

int set_env(WordList *arg);
int unset_env(WordList *arg);
int getenv_internal(WordList *arg);

#endif //SHELDON_BUILTINS_ENV_H
