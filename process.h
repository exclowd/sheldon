//
// Created by kannav on 9/3/20.
//

#ifndef SHELDON_PROCESS_H
#define SHELDON_PROCESS_H

#include <signal.h>
#include <wait.h>
#include "utils.h"

typedef struct PROCESS_NODE {
    int pid;
    int id;
    int status; // 0 for bg 1 for fg -1 exited
    char * command;
    struct PROCESS_NODE * next;
} simple_process;

typedef struct PROCESS_LIST {
    struct PROCESS_NODE * head;
} process_list;

extern int number_of_bg_processes;

extern simple_process * head;

int init_bg_proc_q(void);

int add_process(int pid, char *command);

void poll_process(void);

void kill_all_bgproc ();

#endif //SHELDON_PROCESS_H