//
// Created by kannav on 9/25/20.
//

#ifndef SHELDON_BUILTINS_ENV_H
#define SHELDON_BUILTINS_ENV_H
#include "../src/command.h"

int set_env(arglist_t *arg);
int unset_env(arglist_t *arg);
int getenv_internal(arglist_t *arg);

#endif  // SHELDON_BUILTINS_ENV_H
