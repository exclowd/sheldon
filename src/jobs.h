//
// Created by kannav on 9/3/20.
//

#ifndef SHELDON_SRC_JOBS_H
#define SHELDON_SRC_JOBS_H

#include "command.h"
#include <sys/types.h>

typedef struct PROCESS_NODE {
  int _pgid;
  int _jobid;
  char *_command;
  struct PROCESS_NODE *_next;
} JobInternal;

typedef struct PROCESS_LIST {
  struct PROCESS_NODE *head;
} JobList;

extern int last_job_id;

extern JobInternal *head;

int init_job_queue(void);

int add_job(int pgid, char *command);

void poll_for_exited_jobs(int sig);

int print_jobs(WordList *args);

int kill_job(WordList *args);

void kill_all_bg_jobs(void);

void put_job_in_fg(JobInternal *j, int cont);

void put_job_in_bg(pid_t pid, int cont);

int fg_job(WordList *args);

int bg_job(WordList *args);

int kill_jobs(WordList *args);

#endif //SHELDON_SRC_JOBS_H