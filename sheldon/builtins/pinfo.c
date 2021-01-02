//
// Created by kannav on 9/23/20.
//

#include "pinfo.h"
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <linux/limits.h>
#include <limits.h>

void get_process_info_internal(pid_t pid) {
  char *proc = (char *) malloc(PATH_MAX);
  char *exec = (char *) malloc(PATH_MAX);
  memset(exec, 0, PATH_MAX);
  sprintf(proc, "/proc/%d/stat", pid);
  FILE *fp = fopen(proc, "r");
  if (fp == NULL) {
	perror("pinfo");
	free(proc);
	free(exec);
	return;
  }
  long unsigned memory = 0;
  char status[] = {'\0', '\0', '\0'};

  for (int i = 0; i < 23; i++) {
	if (i == 2) {
	  fscanf(fp, "%1s", status);
	} else if (i == 22) {
	  fscanf(fp, "%lu", &memory);
	} else {
	  fscanf(fp, "%*s");
	}
  }

  if (getpgid(pid) == tcgetpgrp(0)) {
	status[1] = '+';
  }

  sprintf(proc, "/proc/%d/exe", pid);

  size_t a = readlink(proc, exec, PATH_MAX);

  if (a >= PATH_MAX) {
	a = PATH_MAX - 1;
  }
  exec[a] = '\0';

  printf("Pid -- %d\n", pid);
  printf("Process Status -- %s\n", status);
  printf("Virtual Memory -- %ld\n", memory);
  printf("Executable Path -- %s\n", exec);

  fclose(fp);

  free(proc);
  free(exec);
}

int get_process_info(ArgsList *args) {
  if (args == (ArgsList *) NULL) {
	get_process_info_internal(getpid());
  } else {
	for (ArgsList *curr = args; curr != NULL; curr = curr->_next) {
	  char *endptr;
	  errno = 0;
	  long pid = strtol(curr->_text, &endptr, 10);
	  if ((errno == ERANGE && (pid == LONG_MAX || pid == LONG_MIN))
		  || (errno != 0 && pid == 0)) {
		perror("pinfo");
		return -1;
	  }

	  if (endptr == curr->_text) {
		fprintf(stderr, "No valid digits were found\n");
		return -1;
	  }

	  get_process_info_internal(pid);
	}
  }
  return 0;
}
