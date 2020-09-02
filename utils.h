//
// Created by kannav on 9/1/20.
//

#ifndef SHELDON_UTILS_H
#define SHELDON_UTILS_H

#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <linux/limits.h>
#include <ctype.h>
#include <errno.h>
#include <sys/utsname.h>

extern char * home;

extern char * pwd;

extern char *inp;

extern char **argv;

void display_prompt(void);

void exit_safely(void);

#endif //SHELDON_UTILS_H
