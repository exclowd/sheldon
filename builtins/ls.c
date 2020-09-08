//
// Created by kannav on 9/8/20.
//

#include "ls.h"

enum filetype
{
    unknown,
    fifo,
    chardev,
    directory,
    blockdev,
    normal,
    symbolic_link,
    sock,
    whiteout,
    arg_directory
};

static char const filetype_letter[] = "?pcdb-lswd";

typedef struct FILEINFO {
    char * name;
    enum filetype filetype;
    int quoted;
} fileinfo;

int print_format_short( fileinfo * file) {
}

int print_format_long( fileinfo * file) {

}

int (*format_functions[])(fileinfo *) = {
        print_format_short,
        print_format_long
};

int enumerate(char *dir, int all, int ll) {
    DIR * dirp;
    printf("short %s %d\n", dir, all);
    struct dirent *next;

    dirp = opendir(dir);

    if (!dirp) {
        printf("Cannot open directory %s", dir);
        return -1;
    }
    while (1) {
        errno = 0;
        next = readdir(dirp);
        if (next) {

        } else if (errno != 0) {
            printf("Error in reading directory %s", dir);
        } else {
            break;
        }
    }
}

int list_files(list_node *args) {
    if (args == NULL) {
        enumerate(pwd, 0, 0);
    }
    int all = 0, ll = 0;
    char c;
    while ((c = getcommand_opt(args, "la")) != -1) {
        switch (c) {
            case 'l':
                ll = 1;
                break;
            case 'a':
                all = 1;
                break;
            case '?':
                printf("Invalid argument: format ls -[a, l]\n");
                return -1;
            default:
                continue;
        }
    }
    if (nonopt == NULL) {
        enumerate(pwd, all, ll);
    } else {
        for (list_node *curr = nonopt; curr != NULL; curr = curr->next) {
            if (*(curr->word->text) != '-') {
                enumerate(curr->word->text, all, ll);
            }
        }
    }
}