//
// Created by kannav on 9/25/20.
//

#include "env.h"

#include "../src/utils.h"

//#include "../src/utils.h"
#include <stdlib.h>

int set_env(arglist_t *args) {
  if (args == NULL || args->_next == NULL) {
    EPRINTF("sheldon: setenv: usage: setenv var [value]\n");
    return -1;
  }
  char *name = args->_text;
  char *val;
  if (args->_next != NULL) {
    val = args->_next->_text;
  } else {
    val = "";
  }
  if (setenv(name, val, 1) == -1) {
    perror("sheldon: setenv");
    return -1;
  }
  return 0;
}

int unset_env(arglist_t *args) {
  if (args == NULL || args->_next != NULL) {
    EPRINTF("sheldon: unsetenv: usage: unsetenv var\n");
    return -1;
  }
  char *name = args->_text;
  if (name != NULL) {
    if (unsetenv(name) == -1) {
      perror("sheldon: unsetenv\n");
      return -1;
    }
    return 0;
  }
  return -1;
}

int getenv_internal(arglist_t *args) {
  if (args == NULL) {
    EPRINTF("sheldon: getenv: usage: getenv args\n");
    return -1;
  }
  for (arglist_t *curr = args; curr != NULL; curr = curr->_next) {
    char *name = curr->_text;
    char *val;
    val = getenv(name);
    if (val != NULL) {
      printf("%s\n", val);
    } else {
      EPRINTF("sheldon: getenv: %s is not an environment variable\n", name);
      return -1;
    }
  }
  return 0;
}