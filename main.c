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

    while (1) {
        display_prompt();
        inp = read_input();
        int len = tokenize_input(&input_argv, inp);
        simple_command *command;
        char ** args;
        for (int i = 0; i < len; i++) {
            command = load_command(input_argv[i]);
            current_command = command;
            execute_command(command);
            free_command(command);
            current_command = (simple_command * ) NULL;
        }
        free(inp);
        free(input_argv);
    }

    free(home);
}
