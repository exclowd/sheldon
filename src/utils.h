//
// Created by kannav on 9/1/20.
//

#ifndef SHELDON_UTILS_H
#define SHELDON_UTILS_H

#include <stdio.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <termios.h>


#define eprintf(...) fprintf(stderr,__VA_ARGS__),fflush(stderr)

extern pid_t shell_pgid;


extern int shell_terminal;

extern int shell_is_interactive;

extern char *home;

extern char *pwd;

extern char *inp;

extern struct winsize terminal;

struct termios orig_termios;

extern char **input_argv;

void exit_successfully(void);

void exit_safely(int return_code);

void exit_abruptly(int return_code);

#endif //SHELDON_UTILS_H
