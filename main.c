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
        int len = tokenize_input(&argv, inp);
        simple_command *command;
        char ** args;
        for (int i = 0; i < len; i++) {
            command = load_command(argv[i]);
            execute_command(command);
            free_command(command);
        }
        free(inp);
        free(argv);
    }

    free(home);
}
