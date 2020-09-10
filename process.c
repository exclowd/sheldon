//
// Created by kannav on 9/3/20.
//

#include "process.h"

process_list *bgproc;

int number_of_bg_processes = 0;

simple_process *createNode(pid_t pid, char *command) {
    simple_process *newNode = malloc(sizeof(simple_process));
    char * str;
    str = (char *) malloc(strlen(command) + 1);
    strncpy(str, command, strlen(command));
    str[strlen(command)] = '\0';
    if (!newNode) {
        return NULL;
    }
    newNode->pid = pid;
    newNode->id = ++number_of_bg_processes;
    newNode->status = 0;
    newNode->command = str;
    newNode->next = NULL;
    return newNode;
}

process_list *makelist() {
    process_list *list = malloc(sizeof(process_list));
    if (!list) {
        return NULL;
    }
    list->head = NULL;
    return list;
}

void display(process_list *list) {
    if (list->head == NULL) {
        return;
    }
    for (simple_process *curr = list->head; curr != NULL; curr = curr->next) {
        printf("%d\n", curr->pid);
    }
}

simple_process *add(int pid, char *command, process_list *list) {
    simple_process *curr = NULL;
    if (list->head == NULL) {
        list->head = createNode(pid, command);
        return list->head;
    } else {
        curr = list->head;
        while (curr->next != NULL) {
            curr = curr->next;
        }
        curr->next = createNode(pid, command);
        return curr->next;
    }
}

void delete(int pid, process_list *list) {
    simple_process *curr = list->head;
    simple_process *previous = curr;
    while (curr != NULL) {
        if (curr->pid == pid) {
            previous->next = curr->next;
            if (curr == list->head) {
                list->head = curr->next;
            }
            free(curr->command);
            free(curr);
            return;
        }
        previous = curr;
        curr = curr->next;
    }
}

simple_process *find(int pid, process_list *list) {
    simple_process *curr = list->head;
    while (curr != NULL) {
        if (curr->pid == pid) {
            return curr;
        }
        curr = curr->next;
    }
    return NULL;
}

void kill_all_bgproc() {
    simple_process *curr = bgproc->head;
    simple_process *next = curr;
    while (curr != NULL) {
        free(curr->command);
        kill(curr->pid, SIGTERM);
        next = curr->next;
        free(curr);
        curr = next;
    }
    free(bgproc);
}

int init_bg_proc_q() {
    bgproc = makelist();
    return (bgproc != NULL);
}

int add_process(int pid, char *command) {
    simple_process *proc = add(pid, command, bgproc);
    return (proc != NULL);
}

void poll_process(void) {
    int status;
    pid_t pid;
    simple_process *proc;
    while (1) {
        pid = waitpid(-1, &status, WNOHANG);
        if (pid < 0 && errno != ECHILD) {
            perror("wait :");
            return;
        } else if (pid > 0) {
            proc = find(pid, bgproc);
            if (proc != NULL) {
                fprintf(stderr, "Process [%d] %s with pid [%d] exited with code: %d\n",
                       proc->id, proc->command, proc->pid, status);
                delete(pid, bgproc);
            }
        } else {
            return;
        }
    }
}

void get_process_info_internal(pid_t pid) {
    char * proc = (char *) malloc(PATH_MAX);
    char * exec = (char *) malloc(PATH_MAX);
    memset(exec, 0, PATH_MAX);
    sprintf(proc,"/proc/%d/stat",pid);
    FILE * fp = fopen(proc, "r");
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
            fscanf(fp,"%1s", status);
        } else if (i == 22) {
            fscanf(fp,"%lu", &memory);
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


    printf("pid -- %d\n", pid);
    printf("Process Status -- %s\n", status);
    printf("memory -- %ld\n", memory);
    printf("Executable Path -- %s\n", exec);

    fclose(fp);

    free(proc);
    free(exec);
}

int get_process_info(list_node * args) {
    if (args == (list_node *) NULL) {
        get_process_info_internal(getpid());
    } else {
        for (list_node * curr = args; curr != NULL; curr = curr->next) {
            char * endptr;
            errno = 0;
            long pid = strtol(curr->word->text, &endptr, 10);
            if ((errno == ERANGE && (pid == LONG_MAX || pid == LONG_MIN))
                || (errno != 0 && pid == 0)) {
                perror("pinfo");
                return -1;
            }

            if (endptr == curr->word->text) {
                fprintf(stderr, "No valid digits were found\n");
                return -1;
            }

            get_process_info_internal(pid);
        }
    }
    return 0;
}