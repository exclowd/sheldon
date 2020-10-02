//
// Created by kannav on 9/3/20.
//

#include "jobs.h"
#include "utils.h"
#include "exec.h"
#include <signal.h>
#include <wait.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <linux/limits.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <termios.h>


#define MAX(a, b) ((a) < (b) ? (b) : (a))
#define MIN(a, b) ((a) > (b) ? (b) : (a))

job_list *jobList = NULL;

int last_job_id = 0;

static job_internal *create_node(int job_id, pid_t pid, char *command) {
	job_internal *new_node = malloc(sizeof(job_internal));
	if (!new_node) {
		return NULL;
	}
	new_node->_jobid = job_id;
	new_node->_pgid = pid;
	new_node->_command = command;
	new_node->_next = NULL;
	return new_node;
}

static job_list *make_list() {
	job_list *list = malloc(sizeof(job_list));
	if (!list) {
		return NULL;
	}
	list->head = NULL;
	return list;
}

static void delete_list(job_list *job) {
	free(job);
}

static char get_proc_status(pid_t pid) {
	char *proc = (char *) malloc(PATH_MAX);
	sprintf(proc, "/proc/%d/stat", pid);
	FILE *fp = fopen(proc, "r");
	if (fp == NULL) {
		free(proc);
		return -1;
	}
	char status = 0;
	for (int i = 0; i < 3; i++) {
		if (i == 2) {
			fscanf(fp, "%c", &status);
		} else {
			fscanf(fp, "%*s");
		}
	}
	fclose(fp);
	free(proc);
	return status;
}

static void display_job_status(job_internal *j) {
	printf("[%d] ", j->_jobid);
	char c = get_proc_status(j->_pgid);
	printf("%c", c);
	switch (c) {
		case 'T': printf("sleeping");
			break;
		case 'Z': printf("Zombie(defunct)");
			break;
		default: printf("running");
			break;
	}
	printf(" %s [%d]\n", j->_command, j->_pgid);
}

static void display_all_jobs(job_list *list) {
	if (list->head == NULL) {
		return;
	}
	for (job_internal *curr = list->head; curr != NULL; curr = curr->_next) {
		display_job_status(curr);
	}
}

static job_internal *add(int pid, char *command, job_list *list) {
	job_internal *curr = NULL;
	int job_id = last_job_id + 1;
	last_job_id = job_id;
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

static void delete(int pid, job_list *list) {
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

static job_internal *find(pid_t pid, job_list *list) {
	job_internal *curr = list->head;
	while (curr != NULL) {
		if (curr->_pgid == pid) {
			return curr;
		}
		curr = curr->_next;
	}
	return NULL;
}

static job_internal *get_job(int jobid, job_list *list) {
	job_internal *curr = list->head;
	while (curr != NULL) {
		if (curr->_jobid == jobid) {
			return curr;
		}
		curr = curr->_next;
	}
	return NULL;
}

int kill_job(word_list *args) {
	if (len(args) != 2) {
		printf("sheldon: kill: invalid arguments\n");
	} else {
		char *ptr;
		int job = strtol(args->_word->_text, &ptr, 10);
		int sig = atoi(args->_next->_word->_text);
		job_internal *j;
		if ((j = get_job(job, jobList)) != NULL) {
			killpg(j->_pgid, sig);
			return 0;
		} else {
			printf("sheldon: kjob: job-id %d does not exists, use jobs to see currently running jobs\n", job);
			return -1;
		}
	}
}

void kill_all_bg_jobs() {
	job_internal *curr = jobList->head;
	job_internal *next;
	while (curr != NULL) {
		free(curr->_command);
		kill(-(curr->_pgid), SIGKILL);
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
		printf("[%d] %s %d\n", new_job->_jobid, new_job->_command, new_job->_pgid);
	}
	return (new_job != NULL);
}

void poll_for_exited_jobs(int sig) {
	int status;
	pid_t pgid;
	job_internal *job;
	if (sig == SIGCHLD) printf("\n");
	while (1) {
		pgid = waitpid(-1, &status, WNOHANG);
		if (pgid < 0 && errno != ECHILD) {
			perror("wait :");
			return;
		} else if (pgid > 0) {
			job = find(pgid, jobList);
			if (job != NULL) {
				printf("[%d] %d ", job->_jobid, job->_pgid);
				if (WIFSIGNALED(status) && WTERMSIG(status)) {
					printf("killed");
				} else if (WEXITSTATUS(status) == EXIT_SUCCESS) {
					printf("done");
				} else {
					printf("exited with code %d", status);
				}
				printf("\t %s\n", job->_command);
				delete(pgid, jobList);
			}
		} else {
			return;
		}
	}
}

int print_jobs(word_list *args) {
	if (args != NULL) {
		fprintf(stderr, "sheldon: jobs: No args needed");
		return -1;
	} else {
		display_all_jobs(jobList);
		return 0;
	}
}

void put_job_in_fg(job_internal *j, int cont) {
	/* Put the job into the foreground.  */
	tcsetpgrp(shell_terminal, j->_pgid);

	/* Send the job a continue signal, if necessary.  */
	if (cont) {
		if (kill(-j->_pgid, SIGCONT) < 0)
			perror("kill (SIGCONT)");
	}

	int wstatus;
	/* Wait for it to report.  */
	waitpid(j->_pgid, &wstatus, 0);

	/* Put the shell back in the foreground.  */
	tcsetpgrp(shell_terminal, shell_pgid);
}

void put_job_in_bg(pid_t pid, int cont) {
	setpgid(pid, pid);
	simple_command *command = current_simple_command;
	add_job(pid, get_complete_command(command->_name, command->_args));
	job_internal *j = find(pid, jobList);
	if (cont) {
		if (kill(-j->_pgid, SIGCONT) < 0)
			perror("kill (SIGCONT)");
	}
	printf("[%d] %d suspended %s\n", j->_jobid, j->_pgid, j->_command);
}