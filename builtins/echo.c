//
// Created by kannav on 9/2/20.
//

#include "echo.h"

int echo(word_list *args) {
	while (args != NULL) {
		if (args->_word->_text != NULL) {
			int quotes = 0;
			int len = strlen(args->_word->_text);
			for (int i = 0; i < len; i++) {
				if (isspace(args->_word->_text[i])) {
					quotes = 1;
					break;
				}
			}
			if (quotes) {
				fprintf(stdout, "%s", args->_word->_text);
			} else {
				fprintf(stdout, "%s ", args->_word->_text);
			}
		}
		args = args->_next;
	}
	printf("\n");
}