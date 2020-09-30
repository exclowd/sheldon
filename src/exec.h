//
// Created by kannav on 9/1/20.
//

#ifndef SHELDON_EXEC_H
#define SHELDON_EXEC_H

#include "command.h"


int execute_simple_command(simple_command *cc, int flag);

int execute_system_command(word *command, word_list *arg, int flag);

int execute_compound_command(compound_command * cc);
#endif //SHELDON_EXEC_H
