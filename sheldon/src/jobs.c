//
// Created by kannav on 9/3/20.
//

#include "jobs.h"

#include <errno.h>
#include <linux/limits.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <wait.h>

#include "command.h"
#include "utils.h"

#define MAX(a, b) ((a) < (b) ? (b) : (a))
#define MIN(a, b) ((a) > (b) ? (b) : (a))

joblist_t *jobList = NULL;

int last_job_id = 0;

static job_t *create_node(int job_id, pid_t pid, char *command) {
  job_t *new_node = malloc(sizeof(job_t));
  if (!new_node) {
    return NULL;
  }
  new_node->_jobid = job_id;
  new_node->_pgid = pid;
  new_node->_command = command;
  new_node->_next = NULL;
  return new_node;
}

static joblist_t *make_list() {
  joblist_t *list = malloc(sizeof(joblist_t));
  if (!list) {
    return NULL;
  }
  list->head = NULL;
  return list;
}

static void delete_list(joblist_t *job) { free(job); }

static char get_proc_status(pid_t pid) {
  char *proc = (char *)malloc(PATH_MAX);
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

static void display_job_status(job_t *j) {
  printf("[%d] ", j->_jobid);
  char c = get_proc_status(j->_pgid);
  switch (c) {
    case 'T':
      printf("sleeping");
      break;
    case 'Z':
      printf("Zombie(defunct)");
      break;
    default:
      printf("running");
      break;
  }
  printf(" %s [%d]\n", j->_command, j->_pgid);
}

static void display_all_jobs(joblist_t *list) {
  if (list->head == NULL) {
    return;
  }
  for (job_t *curr = list->head; curr != NULL; curr = curr->_next) {
    display_job_status(curr);
  }
}

static job_t *add(int pid, char *command, joblist_t *list) {
  job_t *curr = NULL;
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

static void delete (int pid, joblist_t *list) {
  job_t *curr = list->head;
  job_t *previous = curr;
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

static job_t *find(pid_t pid, joblist_t *list) {
  job_t *curr = list->head;
  while (curr != NULL) {
    if (curr->_pgid == pid) {
      return curr;
    }
    curr = curr->_next;
  }
  return NULL;
}

static job_t *get_job(int jobid, joblist_t *list) {
  job_t *curr = list->head;
  while (curr != NULL) {
    if (curr->_jobid == jobid) {
      return curr;
    }
    curr = curr->_next;
  }
  return NULL;
}

int kill_job(arglist_t *args) {
  if (len(args) != 2) {
    printf("sheldon: kill: invalid arguments\n");
  } else {
    char *ptr;
    int job = strtol(args->_text, &ptr, 10);
    int sig = atoi(args->_next->_text);
    job_t *j;
    if ((j = get_job(job, jobList)) != NULL) {
      killpg(j->_pgid, sig);
      return 0;
    } else {
      printf(
          "sheldon: kjob: job-id %d does not exists, use jobs to see currently "
          "running jobs\n",
          job);
      return -1;
    }
  }
  return 0;
}

void kill_all_bg_jobs(void) {
  job_t *curr = jobList->head;
  job_t *next;
  while (curr != NULL) {
    free(curr->_command);
    kill(-(curr->_pgid), SIGKILL);
    next = curr->_next;
    free(curr);
    curr = next;
  }
}

int kill_jobs(arglist_t *args) {
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
  job_t *new_job = add(pgid, command, jobList);
  if (new_job != NULL) {
    printf("[%d] %s %d\n", new_job->_jobid, new_job->_command, new_job->_pgid);
  }
  return (new_job != NULL);
}

void poll_for_exited_jobs(int sig) {
  int status;
  pid_t pgid;
  job_t *job = NULL;
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
        delete (pgid, jobList);
      }
      fflush(stdout);
    } else {
      fflush(stdout);
      return;
    }
  }
}

int print_jobs(arglist_t *args) {
  if (args != NULL) {
    fprintf(stderr, "sheldon: jobs: No args needed\n");
    return -1;
  } else {
    display_all_jobs(jobList);
    return 0;
  }
}

void put_job_in_fg(job_t *j, int cont) {
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
    delete (j->_pgid, jobList);
    fflush(stdout);
  }

  poll_for_exited_jobs(0);
}

void put_job_in_bg(pid_t pid, int cont) {
  setpgid(pid, pid);
  command_t *command = current_simple_command;
  add_job(pid, get_complete_command(command->_name, command->_args));
  job_t *j = find(pid, jobList);
  if (cont) {
    if (kill(-j->_pgid, SIGCONT) < 0) perror("kill (SIGCONT)");
  }
  printf("[%d] %d suspended %s\n", j->_jobid, j->_pgid, j->_command);
}

int fg_job(arglist_t *args) {
  int n = len(args);
  if (n != 1) {
    printf("sheldon: job: expected 1 argument found %d\n", n);
    return -1;
  }

  int job_id = atoi(args->_text);
  job_t *j;
  if ((j = get_job(job_id, jobList)) != (job_t *)NULL) {
    put_job_in_fg(j, 1);
    return 0;
  } else {
    printf(
        "sheldon: bg: job-id %d does not exists, use jobs to see currently "
        "running jobs\n",
        job_id);
    return -1;
  }
  return 0;
}

int bg_job(arglist_t *args) {
  int n = len(args);
  if (n != 1) {
    printf("sheldon: job: expected 1 argument found %d\n", n);
    return -1;
  }
  int job_id = atoi(args->_text);
  job_t *j;
  if ((j = get_job(job_id, jobList)) != (job_t *)NULL) {
    if (kill(-j->_pgid, SIGCONT) < 0) perror("kill (SIGCONT)");
    return 0;
  } else {
    printf(
        "sheldon: bg: job-id %d does not exists, use jobs to see currently "
        "running jobs\n",
        job_id);
    return -1;
  }
}
