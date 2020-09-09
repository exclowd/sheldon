//
// Created by kannav on 9/8/20.
//

#include <pwd.h>
#include <grp.h>
#include <time.h>
#include "ls.h"

#define MAX(a, b) ((a) < (b) ? (b) : (a))

enum filetype {
    unknown,
    fifo,
    chardev,
    directory,
    blockdev,
    normal,
    symbolic_link,
    socket,
};

static char const filetype_letter[] = "?pcdb-ls";

typedef struct FILE_INFO {
    char *name;
    enum filetype filetype;
} fileinfo;

fileinfo *files_under;

static void attach(char *dest, const char *dirname, const char *name) {

    const char *pathname = dirname;

    if (dirname[0] != '.' || dirname[1] != 0) {
        while (*pathname) {
            *dest = *pathname;
            dest++;
            pathname++;
        }
        if (pathname > dirname && pathname[-1] != '/') {
            *dest = '/';
            dest++;
        }
    }
    while (*name) {
        *dest = *name;
        dest++;
        name++;
    }
    *dest = 0;
}


static int width = 0;

static void print_format_short(char *dir, fileinfo *file) {
    if (file->filetype == directory) {
        printf("\e[1m%.*s\e[0m ", width, file->name);
    } else {
        printf("%.*s ", width, file->name);
    }
}

static void print_format_long(char *dir, fileinfo *file) {
    char *pathname;
    pathname = (char *) malloc(sizeof(dir) + sizeof(file->name) + 2);
    attach(pathname, dir, file->name);
    struct stat buf;
    if (lstat(pathname, &buf) == 0) {
        printf("%c", filetype_letter[file->filetype]);

        char perm[10];
        memset(perm, 0, sizeof(perm));

        int masks[] = {
                S_IRUSR, S_IWUSR, S_IXUSR,
                S_IRGRP, S_IWGRP, S_IXGRP,
                S_IROTH, S_IWOTH, S_IXOTH
        };

        char c[] = {'r', 'w', 'x'};

        for (int i = 0; i < 9; i++) {
            perm[i] = !!(buf.st_mode & masks[i]) ? c[i % 3] : '-';
        }

        printf("%s ", perm);

        char *time = (char *) malloc(256);

        ctime_r(&buf.st_mtime, time);
        time[16] = 0;

        char *username = getpwuid(buf.st_uid)->pw_name;
        char *groupname = getgrgid(buf.st_gid)->gr_name;

        int usernamelen = (int) strlen(username);
        int groupnamelen = (int) strlen(username);

        printf("%3ld %.*s %.*s %10ld %s ",
               buf.st_nlink, usernamelen + 1, username,
               groupnamelen + 1, groupname, buf.st_size, time + 4);

        free(time);

        if (file->filetype == directory) {
            printf("\e[1m%.*s\e[0m\n", width, file->name);
        } else {
            printf("%.*s\n", width, file->name);
        }

    } else {
        perror("ls");
        free(pathname);
        return;
    }

    free(pathname);
}

void (*format_functions[])(char *dir, fileinfo *file) = {
        print_format_short,
        print_format_long
};

static int remove_hidden(const struct dirent *dir) {
    if (*(dir->d_name) == '.') {
        return 0;
    } else {
        return 1;
    }
}

static int load_file(char *dir, char *name, int i, int ll, long *total) {
    char *pathname;
    pathname = (char *) malloc(sizeof(dir) + sizeof(name) + 2);
    files_under[i].name = name;
    files_under[i].filetype = unknown;
    attach(pathname, dir, name);
    struct stat buf;
    if (lstat(pathname, &buf) == 0) {
        switch (buf.st_mode & S_IFMT) {
            case S_IFBLK:
                files_under[i].filetype = blockdev;
                break;
            case S_IFCHR:
                files_under[i].filetype = chardev;
                break;
            case S_IFDIR:
                files_under[i].filetype = directory;
                break;
            case S_IFIFO:
                files_under[i].filetype = fifo;
                break;
            case S_IFLNK:
                files_under[i].filetype = symbolic_link;
                break;
            case S_IFREG:
                files_under[i].filetype = normal;
                break;
            case S_IFSOCK:
                files_under[i].filetype = socket;
                break;
            default:
                files_under[i].filetype = unknown;
                break;
        }
        *total += buf.st_blocks;
    } else {
        perror("ls");
        free(pathname);
        return -1;
    }
    free(pathname);
    return 0;
}

static int enumerate(char *dir, int all, int ll) {
    struct dirent **namelist;
    long total = 0;

    int n;
    width = 0;

    n = (all ? scandir(dir, &namelist, NULL, alphasort) :
         scandir(dir, &namelist, remove_hidden, alphasort));

    if (n == -1) {
        perror("ls");
        return -1;
    }

    files_under = (fileinfo *) malloc(sizeof(fileinfo) * (n + 1));

    for (int i = 0; i < n; i++) {
        if (load_file(dir, namelist[i]->d_name, i, ll, &total) == -1) {
            free(files_under);
            free(namelist);
            return -1;
        }
        width = MAX(width, strlen(files_under[i].name) + 5);
    }

    if (ll) {
        printf("total %ldK\n", total >> 1);
    }

    for (int i = 0; i < n; i++) {
        format_functions[ll](dir, &files_under[i]);
        free(namelist[i]);
    }

    free(files_under);

    free(namelist);


    printf("\n");
    return 0;
}

int list_files(list_node *args) {
    if (args == NULL) {
        enumerate(".", 0, 0);
        return 0;
    }
    int all = 0, ll = 0;
    char c;
    reset_getcommand_opt();
    while ((c = (char) getcommand_opt(args, "la")) != -1) {
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
        enumerate(".", all, ll);
    } else {
        for (list_node *curr = nonopt; curr != NULL; curr = curr->next) {
            if (*(curr->word->text) != '-') {
                if (enumerate(curr->word->text, all, ll) == -1) {
                    return -1;
                }
            }
        }
    }
    return 0;
}