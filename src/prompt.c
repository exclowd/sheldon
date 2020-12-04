//
// Created by kannav on 10/2/20.
//

#include "prompt.h"
#include "utils.h"
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <linux/limits.h>
#include <sys/utsname.h>

void display_prompt(void) {
  static char temp_directory[PATH_MAX];
  static char buf[100];
  struct utsname machine;
  uname(&machine);
  getlogin_r(buf, sizeof(buf));
  buf[99] = '\0';

  printf("\e[1m%s\e[0m@%s ", buf, machine.nodename);

  int l = (int) strlen(home);
  if (l > 1 && strncmp(home, pwd, l) == 0 && (!pwd[l] || pwd[l] == '/')) {
	strncpy(temp_directory + 1, pwd + l, sizeof(temp_directory) - 2);
	temp_directory[0] = '~';
	temp_directory[sizeof(temp_directory) - 1] = '\0';
	printf("\e[1m%s\e[0m $ ", temp_directory);
  } else {
	printf("\e[1m%s\e[0m $ ", pwd);
  }

  fflush(stdout);
}

