//
// Created by kannav on 9/3/20.
//

#include "process.h"

process_list *bgproc;

int number_of_bg_processes = 0;

simple_process *createnode(pid_t pid, char *command) {
    simple_process *newNode = malloc(sizeof(simple_process));
    char * str = (char *) malloc(strlen(command) + 1);
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
    simple_process *current = list->head;
    if (list->head == NULL)
        return;

    for (; current != NULL; current = current->next) {
        printf("%d\n", current->pid);
    }
}

simple_process *add(int pid, char *command, process_list *list) {
    simple_process *current = NULL;
    if (list->head == NULL) {
        list->head = createnode(pid, command);
        return list->head;
    } else {
        current = list->head;
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = createnode(pid, command);
        return current->next;
    }
}

void delete(int pid, process_list *list) {
    simple_process *current = list->head;
    simple_process *previous = current;
    while (current != NULL) {
        if (current->pid == pid) {
            previous->next = current->next;
            if (current == list->head)
                list->head = current->next;
            free(current);
            return;
        }
        previous = current;
        current = current->next;
    }
}

simple_process *find(int pid, process_list *list) {
    simple_process *current = list->head;
    while (current != NULL) {
        if (current->pid == pid) {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

void kill_all_bgproc() {
    simple_process *current = bgproc->head;
    simple_process *next = current;
    while (current != NULL) {
        kill(current->pid, SIGTERM);
        free(current->command);
        next = current->next;
        free(current);
        current = next;
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
                printf("Process [%d] %s [%d] exited with code: %d",
                       proc->id, proc->command, proc->pid, status);
                delete(pid, bgproc);
            }
        } else {
            return;
        }
    }
}

void pinfo(pid_t pid) {

}