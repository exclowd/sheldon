//
// Created by kannav on 9/1/20.
//

#ifndef SHELDON_SRC_EXEC_H
#define SHELDON_SRC_EXEC_H

#include "command.h"

extern int execute_compound_command(CompoundCommand *cc);

extern pid_t child_pgid;

#endif //SHELDON_SRC_EXEC_H
