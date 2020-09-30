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
#include <errno.h>
#include <sys/utsname.h>
#include <sys/ioctl.h>
#include <termios.h>

#define eprintf(...) fprintf(stderr,__VA_ARGS__),fflush(stderr)

extern pid_t shell_pgid;

extern struct termios shell_tmodes;

extern int shell_terminal;

extern int shell_is_interactive;

extern char *home;

extern char *pwd;

extern char *inp;

extern char **input_argv;

extern struct winsize terminal;

void exit_successfully(void);

void exit_safely(int return_code);

void exit_abruptly(int return_code);

void init_terminal();

#endif //SHELDON_UTILS_H
