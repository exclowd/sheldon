//
// Created by kannav on 9/3/20.
//

#ifndef SHELDON_PROCESS_H
#define SHELDON_PROCESS_H

#include <signal.h>
#include <wait.h>
#include "command.h"
#include "utils.h"

typedef struct PROCESS_NODE {
    int _pgid;
    int _jobid;
    int _status; // 0 for bg 1 for fg -1 exited
    char * _command;
    struct PROCESS_NODE * _next;
} job_internal;

typedef struct PROCESS_LIST {
    struct PROCESS_NODE * head;
} job_list;

extern int last_job_id;

extern job_internal * head;

int init_job_queue(void);

int add_job(int pgid, char *command);

void poll_jobs(void);

void kill_all_bg_jobs ();

#endif //SHELDON_PROCESS_H