//
// Created by kannav on 9/1/20.
//

#include "exec.h"

const char *builtins[] = {
        "cd",
        "pwd",
        "echo",
        "pinfo",
        "ls",
        "exit"
};

int (*builtin_functions[])(word_list *arg) = {
    change_directory,
    print_current_working_directory,
    echo,
    get_process_info,
    list_files_internal,
    (int (*)(word_list *)) exit_successfully
};

int execute_system_command(word *command, word_list *arg, int flag) {
    pid_t pid_1;
    char **argv;
    int status = 0;

    switch ((pid_1 = fork())) {

        case -1: // definitely parent
            perror("fork");
            return -1;
        case 0: // is the child
            argv = generate_argv(command, arg, 0);
            if (flag == 1) { // if background
                setpgid(0, 0);
            }
            execvp(command->_text, argv);
            perror("sheldon : _command");
            free(argv);
            exit_abruptly(1);
    }

    if (pid_1 > 0) {
        if (flag == 1) {
            int res = add_process(pid_1, command->_text);
            if (res) {
                printf("\n[%d] %s %d\n", number_of_bg_processes, command->_text, pid_1);
            } else {
                return -1;
            }
        } else {
            waitpid(0, &status, 0);
            return 0;
        }
    }

    return -1;
}

int execute_command(simple_command *cc) {

    word *command = cc->_name;
    word_list *arg = cc->_args;

    int ret = 0;

    if (command == NULL || command->_text == NULL || strlen(command->_text) == 0) {
        printf("(null) _command does not exist\n");
        return -1;
    }

    register int found = 0;

    int len = sizeof(builtins) / sizeof(char *);

    for (int i = 0; i < len; i++) {
        if (strcmp(command->_text, builtins[i]) == 0) {
            found = 1;
            builtin_functions[i](arg);
            break;
        }
    }

    if (!found) {
//        ret = execute_system_command(cc->_name, cc->_args, cc->flag);
    }

    return ret;
}


