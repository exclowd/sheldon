//
// Created by kannav on 9/8/20.
//

#include "ls.h"

#define MAX(a, b) ((a) < (b) ? (b) : (a))
#define MIN(a, b) ((a) > (b) ? (b) : (a))

int curr_line_width = 0;

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
  char *dir;
  int is_quoted;
  mode_t mode;
  enum filetype filetype;
} file_info;

file_info *files_under;

static void attach(char *pathname, char *dirname, const char *name) {
    char *ptr = dirname;
    if (dirname[0] != '.' || dirname[1] != 0) {
        while (*ptr) {
            *pathname++ = *ptr++;
        }
        if (ptr > dirname && ptr[-1] != '/') {
            *pathname++ = '/';
        }
    }
    while (*name) {
        *pathname++ = *name++;
    }
    *pathname = 0;
}

/*
 * Formating short
 */
static int width = 0;

static void print_format_short(file_info *file) {
    // Decide to go to the next line if current line won't accommodate
    if (curr_line_width != 0 && curr_line_width + width > terminal.ws_col) {
        printf("\n");
        curr_line_width = 0;
    }

    // Set color of the file
    char *color;

    switch (file->filetype) {
        case directory:color = "[01;34m";
            break;
        case symbolic_link:color = "[01;36m";
            break;
        case fifo:color = "[01;33m";
            break;
        case normal:
            if (file->mode & S_IEXEC) {
                color = "[01;32m";
            } else {
                color = "[0m";
            }
            break;
        default:color = "[0m";
            break;
    }

    if (file->is_quoted) {
        printf("\e%s'%s'\e[0m%-*s", color, file->name, width - (int) strlen(file->name) - 2, "");
    } else {
        printf("\e%s%-*s\e[0m", color, width, file->name);
    }

    curr_line_width += width;

    if (curr_line_width > terminal.ws_col) {
        printf("\n");
        curr_line_width = 0;
    }
}

/*
 * Formatting Long
 */
static void print_format_long(file_info *file) {
    // fetch the full pathname of the file
    char *pathname;
    pathname = (char *) malloc(strlen(file->dir) + strlen(file->name) + 4);
    attach(pathname, file->dir, file->name);

    // buffer to store the value of the file variables
    struct stat buf;

    if (lstat(pathname, &buf) == 0) {
        printf("%c", filetype_letter[file->filetype]);

        char permissions[10];
        memset(permissions, 0, sizeof(permissions));

        unsigned int masks[] = {
            S_IRUSR, S_IWUSR, S_IXUSR,
            S_IRGRP, S_IWGRP, S_IXGRP,
            S_IROTH, S_IWOTH, S_IXOTH
        };

        unsigned char c[] = {'r', 'w', 'x'};

        for (int i = 0; i < 9; i++) {
            permissions[i] = !!(file->mode & masks[i]) ? c[i % 3] : '-';
        }

        printf("%s ", permissions);

        char *time = (char *) malloc(256);

        ctime_r(&buf.st_mtime, time);
        time[16] = 0;

        char *username = getpwuid(buf.st_uid)->pw_name;
        char *groupname = getgrgid(buf.st_gid)->gr_name;

        int len_username = (int) strlen(username);
        int len_groupname = (int) strlen(username);

        printf("%3ld %-*s %-*s %8ld %s ",
               buf.st_nlink, len_username + 1, username,
               len_groupname + 1, groupname, buf.st_size, time + 4);

        free(time);

        // Set color of the file
        char *color;

        switch (file->filetype) {
            case directory:color = "[01;34m";
                break;
            case symbolic_link:color = "[01;36m";
                break;
            case fifo:color = "[01;33m";
                break;
            case normal:
                if (file->mode & S_IEXEC) {
                    color = "[01;32m";
                } else {
                    color = "[0m";
                }
                break;
            default:color = "[0m";
                break;
        }

        if (file->is_quoted) {
            printf("\e%s'%s'\e[0m%-*s", color, file->name, width - (int) strlen(file->name) - 2, "");
        } else {
            printf("\e%s%-*s\e[0m", color, width, file->name);
        }
    } else {
        perror("ls");
        free(pathname);
        return;
    }

    printf("\n");

    free(pathname);
}

// Easily call format functions
void (*format_functions[])(file_info *file) = {
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

static int load_file_info(char *dir, char *name, int i, int ll, long *total) {
    char *pathname;

    pathname = (char *) malloc(strlen(dir) + strlen(name) + 4);

    if (strchr(name, ' ') != NULL) {
        files_under[i].is_quoted = 1;
    } else {
        files_under[i].is_quoted = 0;
    }

    files_under[i].name = name;
    files_under[i].dir = dir;
    files_under[i].filetype = unknown;

    attach(pathname, dir, name);

    struct stat buf;

    if (lstat(pathname, &buf) == 0) {

        switch (buf.st_mode & S_IFMT) {
            case S_IFBLK:files_under[i].filetype = blockdev;
                break;
            case S_IFCHR:files_under[i].filetype = chardev;
                break;
            case S_IFDIR:files_under[i].filetype = directory;
                break;
            case S_IFIFO:files_under[i].filetype = fifo;
                break;
            case S_IFLNK:files_under[i].filetype = symbolic_link;
                break;
            case S_IFREG:files_under[i].filetype = normal;
                break;
            case S_IFSOCK:files_under[i].filetype = socket;
                break;
            default:files_under[i].filetype = unknown;
                break;
        }

        files_under[i].mode = buf.st_mode;

        *total += buf.st_blocks;
    } else {

        perror("ls");
        free(pathname);
        return -1;
    }

    free(pathname);

    return 0;
}

static int enumerate_files_in_dir(char *dir, int all, int ll) {
    struct dirent **namelist;
    long total = 0;

    int n;
    curr_line_width = width = 0;

    errno = 0;

    n = (all ? scandir(dir, &namelist, NULL, alphasort) :
         scandir(dir, &namelist, remove_hidden, alphasort));

    if (n == -1) {
        return -1;
    } else if (n == 0) {
        return 0;
    }

    files_under = (file_info *) malloc(sizeof(file_info) * (n + 1));

    for (int i = 0; i < n; i++) {
        if (load_file_info(dir, namelist[i]->d_name, i, ll, &total) == -1) {
            free(files_under);
            free(namelist);
            return -1;
        }
        width = MAX(width, strlen(files_under[i].name) + 2 + 1);
    }

    if (ll) {
        printf("total %ldK\n", total / 2);
    }

    for (int i = 0; i < n; i++) {
        format_functions[ll](&files_under[i]);
        free(namelist[i]);
    }

    free(files_under);

    free(namelist);

    printf("\n");
    return 0;
}

static int list_single_file(char *file, int all, int ll) {
    files_under = (file_info *) malloc(sizeof(file_info) * (1));

    long total = 0;

    if (load_file_info(".", file, 0, ll, &total) == -1) {
        free(files_under);
        return -1;
    }
    width = strlen(files_under[0].name) + 2 + 1;

    format_functions[ll](&files_under[0]);

    free(files_under);
    printf("\n");
    return 0;
}

static int is_dir(char *dir) {
    struct stat dir_stat;
    if (stat(dir, &dir_stat) != 0)
        return 0;
    return S_ISDIR(dir_stat.st_mode);
}

int list_files_internal(list_node *args) {
    if (args == NULL) {
        enumerate_files_in_dir(".", 0, 0);
        return 0;
    }
    int all = 0, ll = 0;
    char c;
    reset_getcommand_opt();
    while ((c = (char) getcommand_opt(args, "la")) != -1) {
        switch (c) {
            case 'l':ll = 1;
                break;
            case 'a':all = 1;
                break;
            case '?':fprintf(stderr, "ls: Invalid argument %s: format ls -[a, l]\n", nonopt->word->text);
                return -1;
            default:continue;
        }
    }
    if (nonopt == NULL) {
        enumerate_files_in_dir(".", all, ll);
    } else {
        int cnt = 0;
        for (list_node *curr = nonopt; curr != NULL; curr = curr->next) {
            if (*(curr->word->text) != '-') {
                cnt++;
            }
        }
        for (list_node *curr = nonopt; curr != NULL; curr = curr->next) {
            if (*(curr->word->text) != '-') {
                if (cnt > 1) printf("%s:\n", curr->word->text);
                if (is_dir(curr->word->text)) {
                    if (enumerate_files_in_dir(curr->word->text, all, ll) == -1) {
                        return -1;
                    }
                } else if (list_single_file(curr->word->text, all, ll) == -1) {
                    return -1;
                }
                if (cnt > 1) printf("\n");
            }
        }
    }
    return 0;
}