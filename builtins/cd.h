//
// Created by kannav on 9/1/20.
//

#ifndef SHELDON_BUILTINS_CD_H
#define SHELDON_BUILTINS_CD_H

#include "../src/command.h"

int change_directory(WordList *dir);

int print_current_working_directory(WordList *dir);

#endif //SHELDON_BUILTINS_CD_H
