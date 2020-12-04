//
// Created by kannav on 9/1/20.
//

#include "utils.h"
#include "command.h"
#include "jobs.h"
#include <stdlib.h>
#include <termios.h>

char *home;

char *pwd;

char *inp;

char **input_argv;

pid_t shell_pgid;

struct winsize terminal;

int shell_terminal = 0;

void exit_successfully() {
  kill_all_bg_jobs();
  free(home);
  free(pwd);
  free(inp);
  free(input_argv);
  free_compound_command(current_command);
  exit(0);
}

void exit_safely(int return_code) {
  kill_all_bg_jobs();
  free(home);
  free(pwd);
  exit(return_code);
}

void exit_abruptly(int return_code) {
  free(home);
  free(pwd);
  free(inp);
  free(input_argv);
  free_compound_command(current_command);
  exit(return_code);
}