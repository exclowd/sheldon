//
// Created by kannav on 9/1/20.
//

#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <signal.h>
#include <termios.h>


#include "exec.h"
#include "builtins.h"
#include "utils.h"
#include "jobs.h"


const char *builtins[] = {
	"cd",
	"pwd",
	"echo",
	"pinfo",
	"ls",
	"exit",
	"setenv",
	"unsetenv",
	"getenv",
	"jobs",
	"kjob",
	"fg",
	"bg",
	"overkill"
};

int (*builtin_functions[])(word_list *arg) = {
	change_directory,
	print_current_working_directory,
	echo,
	get_process_info,
	list_files_internal,
	(int (*)(word_list *)) exit_successfully,
	set_env,
	unset_env,
	getenv_internal,
	print_jobs,
	kill_job,
	fg_job,
	bg_job,
	kill_jobs
};

pid_t child_pgid = -1;

static int execute_system_command(word *command, word_list *arg, int flag) {
	pid_t child_pid;
	char **argv;
	int status = 0;
	if ((child_pid = fork()) == -1) {
		/*the fork failed*/
		perror("fork");
		return -1;
	} else if (child_pid == 0) {
		/*This is the child*/
		signal (SIGINT, SIG_DFL);
		signal (SIGQUIT, SIG_DFL);
		signal (SIGTSTP, SIG_DFL);
		signal (SIGTTIN, SIG_DFL);
		signal (SIGTTOU, SIG_DFL);
		signal (SIGCHLD, SIG_DFL);

		int pid = getpid();
		int pgid = child_pgid;
		
		argv = generate_argv(command, arg, 0);
		
		if (pgid == -1) {
			pgid = pid;
		}
		
		setpgid(pid, pgid);
		if (!flag) {
			tcsetpgrp(shell_terminal, pgid); // make myself incharge
		}
		
		execvp(command->_text, argv);
		
		if (errno == ENOENT) {
			printf("sheldon: command not found %s\n", command->_text);
		} else {
			perror("sheldon: command");
		}
		
		free(argv);
		exit_abruptly(1);
	}
	/*what the parent should do*/
	if (child_pid > 0) {
		/*if what we forked was a background process, stop*/
		if (flag == 1) {
			if (add_job(child_pid, get_complete_command(command, arg))) {
				return 0;
			} 
			return -1;
		} else { // forground
			if (child_pgid == -1) child_pgid = child_pid;
			signal(SIGTTIN, SIG_IGN);
			signal(SIGTTOU, SIG_IGN);
			signal(SIGCHLD, SIG_DFL);

			setpgid(child_pid, child_pgid);
			tcsetpgrp(shell_terminal, child_pgid);
			
			waitpid(-child_pgid, &status, WUNTRACED);

			/*grab back control of the shell*/
			tcsetpgrp(shell_terminal, shell_pgid);
  			tcsetattr (shell_terminal, TCSAFLUSH, &orig_termios) ;


			fflush(stdout);

			signal(SIGTTIN, SIG_DFL);
			signal(SIGTTOU, SIG_DFL);
			signal(SIGCHLD, poll_for_exited_jobs);
			
			if (WIFSTOPPED(status)) {
				put_job_in_bg(child_pgid, 0);
			}

			poll_for_exited_jobs(0);
			return 0;
		}
	}

	return -1;
}

static int execute_simple_command(simple_command *cc, int flag) {
	word *command = cc->_name;
	word_list *arg = cc->_args;

	int ret = 0;

	if (command == NULL || command->_text == NULL || strlen(command->_text) == 0) {
		printf("(null) command does not exist\n");
		return -1;
	}

	int found = 0;

	int len = sizeof(builtins) / sizeof(char *);

	for (int i = 0; i < len; i++) {
		if (strcmp(command->_text, builtins[i]) == 0) {
			found = 1;
			builtin_functions[i](arg);
			break;
		}
	}

	if (!found) {
		ret = execute_system_command(cc->_name, cc->_args, flag);
	}

	return ret;
}

int execute_compound_command(compound_command *cc) {
	int saved_stdin = dup(STDIN_FILENO);
	int saved_stdout = dup(STDOUT_FILENO);

	int input_fd;
	if (cc->_inputFile != NULL) {
		input_fd = open(cc->_inputFile, O_RDONLY);
		if (input_fd == -1) {
			perror("sheldon: open");
			return -1;
		}
	} else {
		input_fd = dup(saved_stdin);
	}
	child_pgid = -1;

	int ret = 0;
	int output_fd;
	for (simple_command_list *curr = cc->_simple_commands; curr != NULL; curr = curr->_next) {
		dup2(input_fd, STDIN_FILENO); // set 0 to correspond to input fd
		close(input_fd); // corresponds to no file now

		current_simple_command = curr->_command;

		if (curr->_next == NULL) {
			if (cc->_outFile != NULL) {
				if (!(cc->_append_input)) {
					output_fd = open(cc->_outFile, O_CREAT | O_TRUNC | O_WRONLY, 0644);
					if (output_fd == -1) {
						perror("sheldon: open");
						return -1;
					}
				} else {
					output_fd =
						open(cc->_outFile, O_CREAT | O_APPEND, 0644);
					if (output_fd == -1) {
						perror("sheldon: open");
						return -1;
					}
				}
			} else {
				output_fd = dup(saved_stdout);
			}
		} else {
			int pipe_fd[2];
			if (pipe(pipe_fd) < 0) {
				perror("pipe");
				return -1;
			}
			output_fd = pipe_fd[1];
			input_fd = pipe_fd[0];
		}

		dup2(output_fd, 1); // set 1 to correspond to output fd
		close(output_fd); // corresponds to no file now

		if (execute_simple_command(curr->_command, cc->_background) != 0) {
			ret = -1;
			break;
		}
	}

	dup2(saved_stdin, STDIN_FILENO); // set 0 to the actual stdin
	dup2(saved_stdout, STDOUT_FILENO); // set 1 to the actual stdout

	close(saved_stdin);
	close(saved_stdout);
	return ret;
}



