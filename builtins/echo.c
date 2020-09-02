//
// Created by kannav on 9/2/20.
//

#include "echo.h"

int echo(list_node* args) {
    while (args != NULL) {
        if (args->word->text != NULL) {
            if (args->word->flag == 1) {
                printf("%s", args->word->text);
            } else {
                printf("%s ", args->word->text);
            }
        }
        args = args->next;
    }
    printf("\n");
}