//
// Created by kannav on 9/25/20.
//

#ifndef SHELDON_BUILTINS_ENV_H_
#define SHELDON_BUILTINS_ENV_H_
#include "../src/command.h"

int set_env(word_list * arg);
int unset_env(word_list * arg);
int getenv_internal(word_list * arg);

#endif //SHELDON_BUILTINS_ENV_H_
