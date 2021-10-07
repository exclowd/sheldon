
#include "parse.h"
#include "input.h"
#include "command.h"
#include "utils.h"
#include "exec.h"
#include "jobs.h"
#include "prompt.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <linux/limits.h>
#include <signal.h>
#include <sys/ioctl.h>
#include <termios.h>

/*for getting the value of system variables*/


void init_terminal() {
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &terminal);
}

void ctrlchandler(int signal) {
	printf("\n");
	fflush(stdout);
	display_prompt();
}

void ctrlzhandler(int signal) {
	printf("\n");
	fflush(stdout);
	display_prompt();
}

void init_shell() {

	home = (char *) malloc(PATH_MAX);
	getcwd(home, PATH_MAX);
	pwd = (char *) malloc(PATH_MAX);
	getcwd(pwd, PATH_MAX);

	if (!init_job_queue()) {
		free(home);
		free(pwd);
		exit(1);
	}

	shell_pgid = getpid();
	if (setpgid(shell_pgid, shell_pgid) < 0) {
		perror("Couldn't put the shell in its own process group");
		exit(1);
	}

	/* Grab control of the terminal.  */
	tcsetpgrp(shell_terminal, shell_pgid);
	tcgetattr(STDIN_FILENO, &orig_termios);
	

	/*set the signal handler to repeat prompt if signal*/
	signal(SIGINT, ctrlchandler);
	signal(SIGTSTP, ctrlzhandler);
	signal(SIGCHLD, poll_for_exited_jobs);
}

int main() {

	init_shell();

	while (1) {
		init_terminal();
		display_prompt();

		
		if ((inp = read_input()) != NULL) {
			/*set the normal signal handling*/

			/*tokenize based on ';'*/
			int len = split_into_commands(&input_argv, inp);

			compound_command *command;

			for (int i = 0; i < len; i++) {
				/*break the string into actual command*/
				command = parser(input_argv[i]);

				if (command != NULL) {
					/*set the global command name to be this command*/
					current_command = command;

					execute_compound_command(command);
					free_compound_command(command);
				}
				current_command = (compound_command *) NULL;
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
