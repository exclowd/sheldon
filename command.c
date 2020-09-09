//
// Created by kannav on 9/1/20.
//

#include "command.h"
#include "parse.h"

simple_command * current_command;

simple_command *load_command(char *line) {
    simple_command *curr;

    curr = (simple_command *) malloc(sizeof(simple_command));

    char *token;

    token = get_next_word(line);

    curr->name = (node *) malloc(sizeof(node));

    curr->args = NULL;
    curr->flag = 0;

    if (!is_quoted) {
        char *pos = strchr(token, '&');
        if (pos != NULL) {
            *pos = '\0';
            if (*(pos++) != '\0') {
                *pos = '\0';
            }
            curr->flag |= 1;
            curr->name->text = token;
            return curr;
        } else {
            curr->name->text = token;
        }
    } else {
        printf("Command name cannot be quoted...");
        return NULL;
    }

    list_node *head;

    while (token != NULL) {
        token = get_next_word(NULL);

        if (token == NULL) break;

        list_node *buf = (list_node *) malloc(sizeof(list_node));
        node *curr_word = (node *) malloc(sizeof(node));

        int is_bg = 0;

        if (!is_quoted) {
            char * pos = strchr(token, '&');
            if (pos != NULL) {
                *pos = '\0';
                if (*(pos++) != '\0') {
                    *pos = '\0';
                }
                is_bg = 1;
            }
        }

        curr->flag |= is_bg;

        if (strlen(token) > 0) {
            buf->word = curr_word;
            curr_word->text = token;
            curr_word->flag = is_quoted;
        } else {
            free(curr_word);
            free(buf);
            continue;
        }

        if (is_bg) {
            break;
        }

        buf->next = NULL;

        if (curr->args == NULL) {
            // connect args to buf
            curr->args = buf;
        } else {
            // connect head to buf
            head->next = buf;
        }

        // reposition head
        head = buf;
    }

    return curr;
}

void free_command(simple_command *command) {
    list_node *head = command->args;
    list_node *tmp;

    while (head != NULL) {
        tmp = head;
        head = head->next;
        free(tmp->word);
        free(tmp);
    }

    free(command->name);
    free(command);
}

int list_length(list_node *list) {
    register int i;
    for (i = 0; list; list = list->next, i++);
    return i;
}

// always remember to free this pointer

char **generate_argv(node * command, list_node *list, int starting_index) {
    int count;
    char **array;

    count = list_length(list);
    array = (char **) malloc((2 + count + starting_index) * sizeof(char *));

    array[0] = command->text;

    for (count = 1; count <= starting_index; count++) {
        array[count] = (char *) NULL;
    }
    for (count = starting_index + 1; list; count++, list = list->next) {
        array[count] = list->word->text;
    }
    array[count] = (char *) NULL;

    return (array);
}

static int idx = 1;

static list_node *head = (list_node *) NULL; // saves last used list

list_node *current = (list_node *) NULL; // the current list node

list_node *nonopt; // start the execution from here

int getcommand_opt(list_node *list, char *opts) {
    char c;

    if (list == 0) {
        nonopt = NULL;
        return -1; // Terminate no arguments
    }

    if (list != head || head == NULL) {
        idx = 1;
        current = head = list;
    }

    if (idx == 1) { // starting a arg
        if (current == NULL) {
            // nothing here
            head = (list_node *) NULL;
            if (nonopt == NULL) {
                nonopt = current; // If it is null then the standard measures are to be taken
            }
            return -1; // Terminate
        }
//        printf("current->word->text %s\n", current->word->text);
        if (*(current->word->text) != '-') {
            if (nonopt == NULL)  {
                nonopt = current;
            }
            current = current->next;
            return 0;
        }
    }


    c = current->word->text[idx];

    if (strchr(opts, c) == NULL) {

        nonopt = current;

        return '?'; // None of the options match not a valid arg
    }


    if (current->word->text[++idx] == '\0') {
        current = current->next;
        idx = 1;
    }

    return c;
}


void reset_getcommand_opt(void) {
    head = current = nonopt = (list_node *) NULL;
}
