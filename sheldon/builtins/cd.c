//
// Created by kannav on 9/1/20.
//

#include "cd.h"

#include <linux/limits.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "../src/utils.h"

int change_directory(arglist_t *dir) {
  if (dir == NULL) {
    chdir(home);
    strcpy(pwd, home);
  } else if (dir->_next) {
    printf("cd: too many arguments\n");
    return -1;
  } else {
    char *directory = dir->_text;
    if (chdir(directory) != 0) {
      perror("cd");
      return -1;
    }
    getcwd(pwd, PATH_MAX);
  }
  return 0;
}

int print_current_working_directory(arglist_t *dir) {
  if (dir == NULL) {
    printf("%s\n", pwd);
  } else {
    printf("pwd : too many arguments\n");
    return -1;
  }
  return 0;
}
