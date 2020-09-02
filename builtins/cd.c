//
// Created by kannav on 9/1/20.
//

#include "cd.h"

int change_directory(list_node * dir) {
    if (dir == NULL) {
        chdir(home);
        strcpy(pwd, home);
    } else if (dir->next) {
        fprintf(stdout, "cd: too many arguments\n");
        return -1;
    } else {
        char * directory = dir->word->text;
        if (chdir(directory) != 0) {
            perror("cd");
            return -1;
        }
        getcwd(pwd, PATH_MAX);
    }
    return 0;
}

int print_current_working_directory(list_node * dir) {
    if (dir == NULL) {
        printf("%s\n", pwd);
    } else {
        fprintf(stdout, "pwd : too many arguments\n");
        return -1;
    }
}
