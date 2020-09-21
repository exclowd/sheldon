//
// Created by kannav on 9/1/20.
//

#ifndef SHELDON_EXEC_H
#define SHELDON_EXEC_H

#include "command.h"
#include "builtins.h"
#include "utils.h"
#include "process.h"

#include <sys/types.h>
#include <sys/wait.h>


int execute_command(simple_command* command);

int execute_system_command(word *command, word_list *arg, int flag);

#endif //SHELDON_EXEC_H
