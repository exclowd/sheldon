//
// Created by kannav on 9/3/20.
//

#include "jobs.h"
#include <signal.h>
#include <wait.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>


#define MAX(a, b) ((a) < (b) ? (b) : (a))
#define MIN(a, b) ((a) > (b) ? (b) : (a))

job_list *jobList = NULL;

int last_job_id = 0;

job_internal *create_node(int job_id, pid_t pid, char *command) {
	job_internal *new_node = malloc(sizeof(job_internal));
	if (!new_node) {
		return NULL;
	}
	new_node->_jobid = job_id;
	new_node->_pgid = pid;
	new_node->_status = 0;
	new_node->_command = command;
	new_node->_next = NULL;
	return new_node;
}

job_list *make_list() {
	job_list *list = malloc(sizeof(job_list));
	if (!list) {
		return NULL;
	}
	list->head = NULL;
	return list;
}

void delete_list(job_list *job) {
	free(job);
}

void display(job_list *list) {
	if (list->head == NULL) {
		return;
	}
	for (job_internal *curr = list->head; curr != NULL; curr = curr->_next) {
		printf("%d\n", curr->_pgid);
	}
}

job_internal *find_job(int job, job_list *list) {
	job_internal *curr = list->head;
	while (curr != NULL) {
		if (curr->_jobid == job) {
			return curr;
		}
		curr = curr->_next;
	}
	return NULL;
}

job_internal *add(int pid, char *command, job_list *list) {
	job_internal *curr = NULL;
	int job_id = 1;
	while (find_job(job_id, list) != NULL) {
		job_id++;
	}
	last_job_id = MAX(job_id, last_job_id);
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

void delete(int pid, job_list *list) {
	job_internal *curr = list->head;
	job_internal *previous = curr;
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

job_internal *find(int pid, job_list *list) {
	job_internal *curr = list->head;
	while (curr != NULL) {
		if (curr->_pgid == pid) {
			return curr;
		}
		curr = curr->_next;
	}
	return NULL;
}

void kill_all_bg_jobs() {
	job_internal *curr = jobList->head;
	job_internal *next;
	while (curr != NULL) {
		free(curr->_command);
		kill(-(curr->_pgid), SIGTERM);
		next = curr->_next;
		free(curr);
		curr = next;
	}
}

int init_job_queue() {
	jobList = make_list();
	return (jobList != NULL);
}

int add_job(int pgid, char *command) {
	job_internal *new_job = add(pgid, command, jobList);
	if (new_job != NULL) {
		printf("\n[%d] %s %d\n", new_job->_jobid, new_job->_command, new_job->_pgid);
	}
	return (new_job != NULL);
}

void poll_jobs(void) {
	int status;
	pid_t pgid;
	job_internal *job;
	while (1) {
		pgid = waitpid(-1, &status, WNOHANG);
		if (pgid < 0 && errno != ECHILD) {
			perror("wait :");
			return;
		} else if (pgid > 0) {
			job = find(pgid, jobList);
			if (job != NULL) {
				fprintf(stderr, "Process [%d] %s with pid [%d] exited with code: %d\n",
				        job->_jobid, job->_command, job->_pgid, status);
				delete(pgid, jobList);
			}
		} else {
			return;
		}
	}
}
