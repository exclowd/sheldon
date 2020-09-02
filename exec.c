//
// Created by kannav on 9/1/20.
//

#include "exec.h"

const char * builtins[] = {
    "cd",
    "pwd",
    "echo",
};

int (*builtin_functions[]) (list_node * arg) = {
    change_directory,
    print_current_working_directory,
    echo
};

int execute_command(node * command, list_node* arg) {

    if (command == NULL) {
        printf("(null) command does not exist\n");
        return -1;
    }

    register int found = 0;

    int len = sizeof(builtins) / sizeof(char *);

    for (int i = 0; i < len; i++) {
        if (strcmp(command->text, builtins[i]) == 0) {
            found = 1;
            builtin_functions[i](arg);
            break;
        }
    }

    if (!found) {
        printf("system command\n");
    }

}
