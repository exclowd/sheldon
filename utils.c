//
// Created by kannav on 9/1/20.
//

#include "utils.h"

char * home;

char * pwd;

char *inp;

char **argv;

static char buf[100];

static char tdir[PATH_MAX];

void exit_safely(void) {
    free(home);
    free(pwd);
    exit(0);
};

void display_prompt(void) {
    struct utsname machine;
    uname(&machine);
    getlogin_r(buf, sizeof(buf));
    buf[99] = '\0';

    printf("\e[1m%s\e[0m@%s ", buf, machine.nodename);
    // good name

    int l = (int) strlen(home);
    if (l > 1 && strncmp(home, pwd, l) == 0  && (!pwd[l] || pwd[l] == '/')) {
        strncpy (tdir + 1, pwd + l, sizeof(tdir) - 2);
        tdir[0] = '~';
        tdir[sizeof(tdir) - 1] = '\0';
        printf("\e[1m%s\e[0m $ ", tdir);
    } else {
        printf("\e[1m%s\e[0m $ ", pwd);
    }
}