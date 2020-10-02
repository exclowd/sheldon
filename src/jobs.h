//
// Created by kannav on 9/3/20.
//

#ifndef SHELDON_PROCESS_H
#define SHELDON_PROCESS_H

#include "command.h"
#include <sys/types.h>

typedef struct PROCESS_NODE {
	int _pgid;
	int _jobid;
	char *_command;
	struct PROCESS_NODE *_next;
} job_internal;

typedef struct PROCESS_LIST {
	struct PROCESS_NODE *head;
} job_list;

extern int last_job_id;

extern job_internal *head;

int init_job_queue(void);

int add_job(int pgid, char *command);

void poll_for_exited_jobs(int sig);

int print_jobs(word_list * args);

int kill_job(word_list * args);

void kill_all_bg_jobs(void);

void put_job_in_fg(job_internal * j, int cont);

void put_job_in_bg(pid_t pid, int cont);

int fg_job(word_list * args);

int bg_job(word_list * args);

int kill_jobs(word_list * args);

#endif //SHELDON_PROCESS_H