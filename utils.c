//
// Created by kannav on 9/1/20.
//

#include "utils.h"
#include "command.h"
#include "process.h"

char *home;

char *pwd;

char *inp;

char **input_argv;

void exit_successfully() {
    kill_all_bgproc();
    free(home);
    free(pwd);
    free(inp);
    free(input_argv);
    free_command(current_command);
    exit(0);
}

void exit_safely(int return_code) {
    kill_all_bgproc();
    free(home);
    free(pwd);
    exit(return_code);
};

void exit_abruptly(int return_code) {
    kill_all_bgproc();
    free(home);
    free(pwd);
    free(inp);
    free(input_argv);
    free_command(current_command);
    exit(return_code);
}

struct winsize terminal;

void init_terminal() {
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &terminal);
}