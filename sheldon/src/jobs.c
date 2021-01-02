//
// Created by kannav on 9/3/20.
//

#include "jobs.h"
#include "utils.h"
#include "command.h"
#include <signal.h>
#include <wait.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <linux/limits.h>
#include <unistd.h>

#define MAX(a, b) ((a) < (b) ? (b) : (a))
#define MIN(a, b) ((a) > (b) ? (b) : (a))

JobList *jobList = NULL;

int last_job_id = 0;

static JobInternal *create_node(int job_id, pid_t pid, char *command) {
  JobInternal *new_node = malloc(sizeof(JobInternal));
  if (!new_node) {
	return NULL;
  }
  new_node->_jobid = job_id;
  new_node->_pgid = pid;
  new_node->_command = command;
  new_node->_next = NULL;
  return new_node;
}

static JobList *make_list() {
  JobList *list = malloc(sizeof(JobList));
  if (!list) {
	return NULL;
  }
  list->head = NULL;
  return list;
}

static void delete_list(JobList *job) {
  free(job);
}

static char get_proc_status(pid_t pid) {
  char *proc = (char *) malloc(PATH_MAX);
  sprintf(proc, "/proc/%d/stat", pid);
  FILE *fp = fopen(proc, "r");
  if (fp == NULL) {
	free(proc);
	return -1;
  }
  char status[] = {'\0', '\0'};
  for (int i = 0; i < 3; i++) {
	if (i == 2) {
	  fscanf(fp, "%1s", status);
	} else {
	  fscanf(fp, "%*s");
	}
  }
  fclose(fp);
  free(proc);
  return status[0];
}

static void display_job_status(JobInternal *j) {
  printf("[%d] ", j->_jobid);
  char c = get_proc_status(j->_pgid);
  switch (c) {
  case 'T': printf("sleeping");
	break;
  case 'Z': printf("Zombie(defunct)");
	break;
  default: printf("running");
	break;
  }
  printf(" %s [%d]\n", j->_command, j->_pgid);
}

static void display_all_jobs(JobList *list) {
  if (list->head == NULL) {
	return;
  }
  for (JobInternal *curr = list->head; curr != NULL; curr = curr->_next) {
	display_job_status(curr);
  }
}

static JobInternal *add(int pid, char *command, JobList *list) {
  JobInternal *curr = NULL;
  int job_id = last_job_id + 1;
  last_job_id = job_id;
  if (list->head == NULL) {
	list->head = create_node(job_id, pid, command);
	return list->head;
  } else {
	curr = list->head;
	while (curr->_next != NULL) {
	  curr = curr->_next;
	}
	curr->_next = create_node(job_id, pid, command);
	return curr->_next;
  }
}

static void delete(int pid, JobList *list) {
  JobInternal *curr = list->head;
  JobInternal *previous = curr;
  while (curr != NULL) {
	if (curr->_pgid == pid) {
	  previous->_next = curr->_next;
	  if (curr == list->head) {
		list->head = curr->_next;
	  }
	  free(curr->_command);
	  free(curr);
	  return;
	}
	previous = curr;
	curr = curr->_next;
  }
}

static JobInternal *find(pid_t pid, JobList *list) {
  JobInternal *curr = list->head;
  while (curr != NULL) {
	if (curr->_pgid == pid) {
	  return curr;
	}
	curr = curr->_next;
  }
  return NULL;
}

static JobInternal *get_job(int jobid, JobList *list) {
  JobInternal *curr = list->head;
  while (curr != NULL) {
	if (curr->_jobid == jobid) {
	  return curr;
	}
	curr = curr->_next;
  }
  return NULL;
}

int kill_job(ArgsList *args) {
  if (len(args) != 2) {
	printf("sheldon: kill: invalid arguments\n");
  } else {
	char *ptr;
	int job = strtol(args->_text, &ptr, 10);
	int sig = atoi(args->_next->_text);
	JobInternal *j;
	if ((j = get_job(job, jobList)) != NULL) {
	  killpg(j->_pgid, sig);
	  return 0;
	} else {
	  printf("sheldon: kjob: job-id %d does not exists, use jobs to see currently running jobs\n", job);
	  return -1;
	}
  }
}

void kill_all_bg_jobs(void) {
  JobInternal *curr = jobList->head;
  JobInternal *next;
  while (curr != NULL) {
	free(curr->_command);
	kill(-(curr->_pgid), SIGKILL);
	next = curr->_next;
	free(curr);
	curr = next;
  }
}

int kill_jobs(ArgsList *args) {
  int n = len(args);
  if (n != 0) {
	printf("sheldon: kjob: No arguments were expected");
	return -1;
  }
  kill_all_bg_jobs();
  delete_list(jobList);
  jobList = make_list();
  return (jobList != NULL);
}

int init_job_queue() {
  jobList = make_list();
  return (jobList != NULL);
}

int add_job(int pgid, char *command) {
  JobInternal *new_job = add(pgid, command, jobList);
  if (new_job != NULL) {
	printf("[%d] %s %d\n", new_job->_jobid, new_job->_command, new_job->_pgid);
  }
  return (new_job != NULL);
}

void poll_for_exited_jobs(int sig) {
  int status;
  pid_t pgid;
  JobInternal *job = NULL;
  while (1) {
	pgid = waitpid(-1, &status, WNOHANG);
	if (pgid < 0 && errno != ECHILD) {
	  perror("wait :");
	  return;
	} else if (pgid > 0) {
	  job = find(pgid, jobList);
	  if (job != NULL) {
		printf("[%d] %d ", job->_jobid, job->_pgid);
		if (WEXITSTATUS(status) == EXIT_SUCCESS) {
		  printf("done");
		} else {
		  printf("exited with code %d", status);
		}
		printf("\t %s\n", job->_command);
		delete(pgid, jobList);
	  }
	  fflush(stdout);
	} else {
	  fflush(stdout);
	  return;
	}
  }
}

int print_jobs(ArgsList *args) {
  if (args != NULL) {
	fprintf(stderr, "sheldon: jobs: No args needed\n");
	return -1;
  } else {
	display_all_jobs(jobList);
	return 0;
  }
}

void put_job_in_fg(JobInternal *j, int cont) {
  signal(SIGTTIN, SIG_IGN);
  signal(SIGTTOU, SIG_IGN);

  tcsetpgrp(shell_terminal, j->_pgid);

  /* Send the job a continue signal, if necessary.  */
  if (cont) {
	if (kill(-j->_pgid, SIGCONT) < 0) {
	  perror("kill (SIGCONT)");
	}
  }

  int wstatus;

  /* Wait for it to report.  */
  waitpid(-j->_pgid, &wstatus, WUNTRACED);

  /* Put the shell back in the foreground.  */
  tcsetpgrp(shell_terminal, shell_pgid);

  signal(SIGTTIN, SIG_DFL);
  signal(SIGTTOU, SIG_DFL);

  if (WIFSTOPPED(wstatus)) {
	printf("[%d] %d suspended %s\n", j->_jobid, j->_pgid, j->_command);
  } else if (WIFEXITED(wstatus)) {
	printf("[%d] %d ", j->_jobid, j->_pgid);
	printf("done");
	printf("\t %s\n", j->_command);
	delete(j->_pgid, jobList);
	fflush(stdout);
  }

  poll_for_exited_jobs(0);
}

void put_job_in_bg(pid_t pid, int cont) {
  setpgid(pid, pid);
  SimpleCommand *command = current_simple_command;
  add_job(pid, get_complete_command(command->_name, command->_args));
  JobInternal *j = find(pid, jobList);
  if (cont) {
	if (kill(-j->_pgid, SIGCONT) < 0)
	  perror("kill (SIGCONT)");
  }
  printf("[%d] %d suspended %s\n", j->_jobid, j->_pgid, j->_command);
}

int fg_job(ArgsList *args) {
  int n = len(args);
  if (n != 1) {
	printf("sheldon: job: expected 1 argument found %d\n", n);
	return -1;
  }

  int job_id = atoi(args->_text);
  JobInternal *j;
  if ((j = get_job(job_id, jobList)) != (JobInternal *) NULL) {
	put_job_in_fg(j, 1);
	return 0;
  } else {
	printf("sheldon: bg: job-id %d does not exists, use jobs to see currently running jobs\n", job_id);
	return -1;
  }
  return 0;
}

int bg_job(ArgsList *args) {
  int n = len(args);
  if (n != 1) {
	printf("sheldon: job: expected 1 argument found %d\n", n);
	return -1;
  }
  int job_id = atoi(args->_text);
  JobInternal *j;
  if ((j = get_job(job_id, jobList)) != (JobInternal *) NULL) {
	if (kill(-j->_pgid, SIGCONT) < 0)
	  perror("kill (SIGCONT)");
	return 0;
  } else {
	printf("sheldon: bg: job-id %d does not exists, use jobs to see currently running jobs\n", job_id);
	return -1;
  }
}
