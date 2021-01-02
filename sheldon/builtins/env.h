//
// Created by kannav on 9/25/20.
//

#ifndef SHELDON_BUILTINS_ENV_H
#define SHELDON_BUILTINS_ENV_H
#include "../src/command.h"

int set_env(ArgsList *arg);
int unset_env(ArgsList *arg);
int getenv_internal(ArgsList *arg);

#endif //SHELDON_BUILTINS_ENV_H
