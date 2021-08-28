#include <linux/limits.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include "command.h"
#include "exec.h"
#include "input.h"
#include "jobs.h"
#include "parse.h"
#include "prompt.h"
#include "utils.h"
/*for getting the value of system variables*/

void init_terminal() { ioctl(STDOUT_FILENO, TIOCGWINSZ, &terminal); }

void ctrl_c_handler(int signal) {
  fflush(stdout);
  printf("\n");
  display_prompt();
}

void ctrl_z_handler(int signal) {
  fflush(stdout);
  display_prompt();
}

void init_shell() {
  home = (char *)malloc(PATH_MAX);
  getcwd(home, PATH_MAX);
  pwd = (char *)malloc(PATH_MAX);
  getcwd(pwd, PATH_MAX);

  if (!init_job_queue()) {
    free(home);
    free(pwd);
    exit(1);
  }

  shell_pgid = getpid();

  /* Grab control of the terminal.  */
  tcsetpgrp(shell_terminal, shell_pgid);

  /*set the signal handler to repeat prompt if signal*/
  signal(SIGINT, ctrl_c_handler);
  signal(SIGTSTP, ctrl_z_handler);
  signal(SIGCHLD, poll_for_exited_jobs);
}

int main() {
  init_shell();

  while (1) {
    init_terminal();
    display_prompt();
    if ((inp = read_input()) != NULL) {
      /*tokenize based on ';'*/
      int len = split_into_commands(&input_argv, inp);
      ccommand_t *command;

      for (int i = 0; i < len; i++) {
        /*break the string into actual command*/
        command = generate_command(input_argv[i]);

        if (command != NULL) {
          /*set the global command name to be this command*/
          current_command = command;

          execute_compound_command(command);
          free_compound_command(command);
        }
        current_command = (ccommand_t *)NULL;
      }

      free(inp);
      free(input_argv);
    } else {
      /*clear the stdin buffer*/
      int c;
      do {
        c = getchar();
      } while (c != '\n' && c != EOF);
    }
  }
}
