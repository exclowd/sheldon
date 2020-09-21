//
// Created by kannav on 9/3/20.
//

#ifndef SHELDON_PROCESS_H
#define SHELDON_PROCESS_H

#include <signal.h>
#include <wait.h>
#include "command.h"
#include "utils.h"
#include <limits.h>

typedef struct PROCESS_NODE {
    int _pid;
    int _id;
    int status; // 0 for bg 1 for fg -1 exited
    char * _command;
    struct PROCESS_NODE * _node;
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

int get_process_info(word_list * args);

#endif //SHELDON_PROCESS_H