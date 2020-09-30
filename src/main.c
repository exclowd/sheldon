#include "parse.h"
#include "input.h"
#include "command.h"
#include "utils.h"
#include "exec.h"

void display_prompt(void) {
	static char tdir[PATH_MAX];
	static char buf[100];
	struct utsname machine;
	uname(&machine);
	getlogin_r(buf, sizeof(buf));
	buf[99] = '\0';

	printf("\e[1m%s\e[0m@%s ", buf, machine.nodename);
	// good _name

	int l = (int) strlen(home);
	if (l > 1 && strncmp(home, pwd, l) == 0 && (!pwd[l] || pwd[l] == '/')) {
		strncpy(tdir + 1, pwd + l, sizeof(tdir) - 2);
		tdir[0] = '~';
		tdir[sizeof(tdir) - 1] = '\0';
		printf("\e[1m%s\e[0m $ ", tdir);
	} else {
		printf("\e[1m%s\e[0m $ ", pwd);
	}
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

	shell_terminal = STDIN_FILENO;
	shell_is_interactive = isatty(shell_terminal);

	if (shell_is_interactive) {
		/* Loop until we are in the foreground.  */
		while (tcgetpgrp(shell_terminal) != (shell_pgid = getpgrp()))
			kill(-shell_pgid, SIGTTIN);

		/* Ignore interactive and job-control signals.  */
		signal(SIGINT, SIG_IGN);
		signal(SIGQUIT, SIG_IGN);
		signal(SIGTSTP, SIG_IGN);
		signal(SIGTTIN, SIG_IGN);
		signal(SIGTTOU, SIG_IGN);
		signal(SIGCHLD, SIG_IGN);

		/* Put ourselves in our own process group.  */
		shell_pgid = getpid();
		if (setpgid(shell_pgid, shell_pgid) < 0) {
			perror("Couldn't put the shell in its own process group");
			exit(1);
		}

		/* Grab control of the terminal.  */
		tcsetpgrp(shell_terminal, shell_pgid);

		/* Save default terminal attributes for shell.  */
		tcgetattr(shell_terminal, &shell_tmodes);
	}
}

int main() {

	init_shell();

	while (1) {
		init_terminal();
		display_prompt();
		if ((inp = read_input()) != NULL) {
			/*tokenize based on ';' and '&&'*/
			int len = split_into_commands(&input_argv, inp);
			compound_command *command;

			for (int i = 0; i < len; i++) {
				/*break the string into actual command*/
				command = Parser(input_argv[i]);
				if (command != NULL) {
					/*set the global command name to be this command*/
					current_command = command;
					execute_compound_command(command);
					free_compound_command(command);
				}
				current_command = (compound_command *) NULL;
			}

			/*get the status of the background jobs*/
			poll_jobs();
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
