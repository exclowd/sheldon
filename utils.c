//
// Created by kannav on 9/1/20.
//

#include "utils.h"
#include "command.h"
#include "process.h"

char * home;

char * pwd;

char *inp;

char **input_argv;

static char buf[100];

static char tdir[PATH_MAX];

void exit_safely(int returncode) {
    kill_all_bgproc();
    free(home);
    free(pwd);
    exit(returncode);
};

void exit_abruptly(int returncode) {
    kill_all_bgproc();
    free(home);
    free(pwd);
    free(inp);
    free(input_argv);
    free_command(current_command);
    exit(returncode);
}

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