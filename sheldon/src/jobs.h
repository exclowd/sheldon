//
// Created by kannav on 9/3/20.
//

#ifndef SHELDON_SRC_JOBS_H
#define SHELDON_SRC_JOBS_H

#include <sys/types.h>

#include "command.h"

typedef struct PROCESS_NODE {
  int _pgid;
  int _jobid;
  char *_command;
  struct PROCESS_NODE *_next;
} job_t;

typedef struct PROCESS_LIST {
  job_t *head;
} joblist_t;

extern int last_job_id;

extern job_t *head;

int init_job_queue(void);

int add_job(int pgid, char *command);

void poll_for_exited_jobs(int sig);

int print_jobs(arglist_t *args);

int kill_job(arglist_t *args);

void kill_all_bg_jobs(void);

void put_job_in_fg(job_t *j, int cont);

void put_job_in_bg(pid_t pid, int cont);

int fg_job(arglist_t *args);

int bg_job(arglist_t *args);

int kill_jobs(arglist_t *args);

#endif  // SHELDON_SRC_JOBS_H