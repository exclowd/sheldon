//
// Created by kannav on 9/23/20.
//

#ifndef SHELDON_BUILTINS_PINFO_H_
#define SHELDON_BUILTINS_PINFO_H_

#include "../src/command.h"
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <linux/limits.h>
#include <limits.h>

int get_process_info(word_list * args);

#endif //SHELDON_BUILTINS_PINFO_H_
