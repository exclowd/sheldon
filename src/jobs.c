//
// Created by kannav on 9/3/20.
//

#include "jobs.h"
#include <signal.h>
#include <wait.h>
#include <errno.h>
#include <stdlib.h>
#include "utils.h"


#define MAX(a, b) ((a) < (b) ? (b) : (a))
#define MIN(a, b) ((a) > (b) ? (b) : (a))

job_list *jobList;

int last_job_id = 0;

job_internal *createNode(int job_id, pid_t pid, char *command) {
	job_internal *newNode = malloc(sizeof(job_internal));
	if (!newNode) {
		return NULL;
	}
	newNode->_jobid = job_id;
	newNode->_pgid = pid;
	newNode->_status = 0;
	newNode->_command = command;
	newNode->_next = NULL;
	return newNode;
}

job_list *makelist() {
	job_list *list = malloc(sizeof(job_list));
	if (!list) {
		return NULL;
	}
	list->head = NULL;
	return list;
}

void deletelist(job_list *job) {
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
		list->head = createNode(job_id, pid, command);
		return list->head;
	} else {
		curr = list->head;
		while (curr->_next != NULL) {
			curr = curr->_next;
		}
		curr->_next = createNode(job_id, pid, command);
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
	job_internal *next = curr;
	while (curr != NULL) {
		free(curr->_command);
		kill(-(curr->_pgid), SIGTERM);
		next = curr->_next;
		free(curr);
		curr = next;
	}
}

int init_job_queue() {
	jobList = makelist();
	return (jobList != NULL);
}

int add_job(int pgid, char *command) {
	job_internal *newJob = add(pgid, command, jobList);
	if (newJob != NULL) {
		printf("\n[%d] %s %d\n", newJob->_jobid, newJob->_command, newJob->_pgid);
	}
	return (newJob != NULL);
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
