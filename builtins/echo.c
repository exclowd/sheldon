//
// Created by kannav on 9/2/20.
//

#include "echo.h"

int echo(word_list* args) {
    while (args != NULL) {
        if (args->_word->_text != NULL) {
            if (args->_word->_flag == 1) {
                printf("%s", args->_word->_text);
            } else {
                printf("%s ", args->_word->_text);
            }
        }
        args = args->_next;
    }
    printf("\n");
}