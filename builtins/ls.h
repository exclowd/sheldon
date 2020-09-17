//
// Created by kannav on 9/8/20.
//

#ifndef SHELDON_LS_H
#define SHELDON_LS_H

#include "../utils.h"
#include "../command.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include "dirent.h"

int list_files_internal(list_node * args);

#endif //SHELDON_LS_H
