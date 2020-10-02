//
// Created by kannav on 9/1/20.
//

#ifndef SHELDON_EXEC_H
#define SHELDON_EXEC_H

#include "command.h"


int execute_compound_command(compound_command * cc);

extern pid_t child_pgid;

#endif //SHELDON_EXEC_H
