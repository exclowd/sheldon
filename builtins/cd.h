//
// Created by kannav on 9/1/20.
//

#ifndef SHELDON_CD_H
#define SHELDON_CD_H

#include "../command.h"
#include "../utils.h"
#include <unistd.h>
#include <linux/limits.h>

int change_directory(list_node * dir);

int print_current_working_directory(list_node * dir);

#endif //SHELDON_CD_H
