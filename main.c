#include "parse.h"
#include "input.h"
#include "command.h"
#include "utils.h"
#include "exec.h"


void display_prompt(void) {
    static char tdir[PATH_MAX];
    static char buf[100];
    struct utsname machine;
    uname(&machine);
    getlogin_r(buf, sizeof(buf));
    buf[99] = '\0';

    printf("\e[1m%s\e[0m@%s ", buf, machine.nodename);
    // good _name

    int l = (int) strlen(home);
    if (l > 1 && strncmp(home, pwd, l) == 0 && (!pwd[l] || pwd[l] == '/')) {
        strncpy(tdir + 1, pwd + l, sizeof(tdir) - 2);
        tdir[0] = '~';
        tdir[sizeof(tdir) - 1] = '\0';
        printf("\e[1m%s\e[0m $ ", tdir);
    } else {
        printf("\e[1m%s\e[0m $ ", pwd);
    }
}



int main() {

    home = (char *) malloc(PATH_MAX);
    getcwd(home, PATH_MAX);
    pwd = (char *) malloc(PATH_MAX);
    getcwd(pwd, PATH_MAX);

//    if (!init_bg_proc_q()) {
//        free(home);
//        free(pwd);
//
//        exit(1);
//    }

    while (1) {
        init_terminal();
        display_prompt();
        inp = read_input();
        int len = split_into_commands(&input_argv, inp);
        compound_command *command;

        for (int i = 0; i < len; i++) {
            command = Parser(input_argv[i]);
            if (command != NULL) {
                current_command = command;
                execute_compound_command(command);
//                free_command(command);
            }
            current_command = (compound_command *) NULL;
        }

//        poll_process();
        free(inp);
        free(input_argv);
    }

}
