//
// Created by kannav on 9/25/20.
//

#include "env.h"
#include <stdlib.h>

int set_env(word_list *  args) {
	if (args == NULL || args->_next->_next != NULL) {
		eprintf("sheldon: setenv: usage: setenv var [value]");
		return -1;
	}
	char * name = args->_word->_text;
	char * val;
	if (args->_next != NULL) {
		val = args->_next->_word->_text;
	} else {
		val = "";
	}
	if (setenv(name, val, 1) == -1) {
		perror("sheldon: setenv");
		return -1;
	}
	return 0;
}


int unset_env(word_list * args) {
	if (args == NULL || args->_next != NULL) {
		eprintf("sheldon: unsetenv: usage: unsetenv var");
		return -1;
	}
	char * name = args->_word->_text;
	if (name != NULL) {
		if (unsetenv(name) == -1) {
			perror("sheldon: unsetenv");
			return -1;
		}
		return 0;
	}
	return -1;
}

int getenv_internal(word_list * args) {
	if (args == NULL) {
		eprintf("sheldon: getenv: usage: getenv args");
		return -1;
	}
	for (word_list * curr = args; curr != NULL; curr = curr->_next) {
		char *name = curr->_word->_text;
		char * val;
		val = getenv(name);
		if (val != NULL) {
			printf("%s\n",val);
		} else {
			eprintf("sheldon: getenv: %s is not an environment variable\n", name);
			return -1;
		}
	}
	return 0;
}