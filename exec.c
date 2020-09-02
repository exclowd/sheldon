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



int execute_in_foreground(node * command, list_node * arg) {

}

int execute_command(simple_command *cc) {

    node * command = cc->name;
    list_node * arg = cc->args;

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
        if (cc->flag) {
            execute_in_foreground(cc->name, cc->args);
        }
    }

}


