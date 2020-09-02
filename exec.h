//
// Created by kannav on 9/1/20.
//

#ifndef SHELDON_EXEC_H
#define SHELDON_EXEC_H

#include "command.h"
#include "builtins.h"

int execute_command(node * command, list_node* arg);

#endif //SHELDON_EXEC_H
