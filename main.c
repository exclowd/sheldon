#include "parse.h"
#include "input.h"
#include "command.h"
#include "utils.h"
#include "exec.h"

int main() {

    home = (char *) malloc(PATH_MAX);
    getcwd(home, PATH_MAX);
    pwd = (char *) malloc(PATH_MAX);
    getcwd(pwd, PATH_MAX);

    if (!init_bg_proc_q()) {
        free(home);
        free(pwd);
        exit(1);
    }

    while (1) {
        display_prompt();
        inp = read_input();
        init_terminal();
        int len = split_into_commands(&input_argv, inp);
        simple_command *command;

        for (int i = 0; i < len; i++) {
            command = load_command(input_argv[i]);
            if (command != NULL) {
                current_command = command;
                execute_command(command);
                free_command(command);
            }
            current_command = (simple_command *) NULL;
        }

        poll_process();
        free(inp);
        free(input_argv);
    }

}
