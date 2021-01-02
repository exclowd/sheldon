//
// Created by kannav on 9/2/20.
//

#include "echo.h"
#include "../src/utils.h"
#include <string.h>
#include <ctype.h>

int echo(ArgsList *args) {
  while (args != NULL) {
	if (args->_text != NULL) {
	  int quotes = 0;
	  size_t len = strlen(args->_text);
	  for (int i = 0; i < len; i++) {
		if (isspace(args->_text[i])) {
		  quotes = 1;
		  break;
		}
	  }
	  if (quotes) {
		fprintf(stdout, "%s", args->_text);
	  } else {
		fprintf(stdout, "%s ", args->_text);
	  }
	}
	args = args->_next;
  }
  printf("\n");
  return 0;
}