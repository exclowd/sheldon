//
// Created by kannav on 9/8/20.
//

#include "ls.h"

#include <dirent.h>
#include <errno.h>
#include <grp.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>

#include "../src/utils.h"

#define MAX(a, b) ((a) < (b) ? (b) : (a))

int curr_line_width = 0;

enum FileType {
  UNKNOWN,
  FIFO,
  CHARDEV,
  DIRECTORY,
  BLOCKDEV,
  NORMAL,
  SYMBOLIC_LINK,
  SOCKET,
};

static char const filetype_letter[] = "?pcdb-ls";

typedef struct FILE_INFO {
  char *name;
  char *dir;
  int is_quoted;
  mode_t mode;
  enum FileType filetype;
} __attribute__((aligned(32))) FileInfo;

FileInfo *files_under;

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

static void print_format_short(FileInfo *file) {
  // Decide to go to the _next line if current line won't accommodate
  if (curr_line_width != 0 && curr_line_width + width > terminal.ws_col) {
    fprintf(stdout, "\n");
    curr_line_width = 0;
  }

  // Set color of the file
  char *color;

  switch (file->filetype) {
    case DIRECTORY:
      color = "[01;34m";
      break;
    case SYMBOLIC_LINK:
      color = "[01;36m";
      break;
    case FIFO:
      color = "[01;33m";
      break;
    case NORMAL:
      if (file->mode & S_IEXEC) {
        color = "[01;32m";
      } else {
        color = "[0m";
      }
      break;
    default:
      color = "[0m";
      break;
  }

  if (file->is_quoted) {
    fprintf(stdout, "\e%s'%s'\e[0m%-*s", color, file->name,
            width - (int)strlen(file->name) - 2, "");
  } else {
    fprintf(stdout, "\e%s%-*s\e[0m", color, width, file->name);
  }

  curr_line_width += width;

  if (curr_line_width > terminal.ws_col) {
    fprintf(stdout, "\n");
    curr_line_width = 0;
  }
}

/*
 * Formatting Long
 */
static void print_format_long(FileInfo *file) {
  // fetch the full pathname of the file
  char *pathname;
  pathname = (char *)malloc(strlen(file->dir) + strlen(file->name) + 4);
  attach(pathname, file->dir, file->name);

  // buffer to store the value of the file variables
  struct stat buf;

  if (lstat(pathname, &buf) == 0) {
    fprintf(stdout, "%c", filetype_letter[file->filetype]);

    unsigned char permissions[10];
    memset(permissions, 0, sizeof(permissions));

    unsigned int masks[] = {S_IRUSR, S_IWUSR, S_IXUSR, S_IRGRP, S_IWGRP,
                            S_IXGRP, S_IROTH, S_IWOTH, S_IXOTH};

    unsigned char c[] = {'r', 'w', 'x'};

    for (int i = 0; i < 9; i++) {
      if (!!(file->mode & masks[i])) {
        permissions[i] = c[i % 3];
      } else {
        permissions[i] = '-';
      }
    }

    fprintf(stdout, "%s ", permissions);

    char *time = (char *)malloc(256);

    ctime_r(&buf.st_mtime, time);
    time[16] = 0;

    char *username = getpwuid(buf.st_uid)->pw_name;
    char *groupname = getgrgid(buf.st_gid)->gr_name;

    int len_username = (int)strlen(username);
    int len_groupname = (int)strlen(username);

    fprintf(stdout, "%3ld %-*s %-*s %8ld %s ", buf.st_nlink, len_username + 1,
            username, len_groupname + 1, groupname, buf.st_size, time + 4);

    free(time);

    // Set color of the file
    char *color;

    switch (file->filetype) {
      case DIRECTORY:
        color = "[01;34m";
        break;
      case SYMBOLIC_LINK:
        color = "[01;36m";
        break;
      case FIFO:
        color = "[01;33m";
        break;
      case NORMAL:
        if (file->mode & S_IEXEC) {
          color = "[01;32m";
        } else {
          color = "[0m";
        }
        break;
      default:
        color = "[0m";
        break;
    }

    if (file->is_quoted) {
      fprintf(stdout, "\e%s'%s'\e[0m%-*s", color, file->name,
              width - (int)strlen(file->name) - 2, "");
    } else {
      fprintf(stdout, "\e%s%-*s\e[0m", color, width, file->name);
    }
  } else {
    perror("ls");
    free(pathname);
    return;
  }

  fprintf(stdout, "\n");

  free(pathname);
}

// Easily call format functions
void (*format_functions[])(FileInfo *file) = {print_format_short,
                                              print_format_long};

static int remove_hidden(const struct dirent *dir) {
  if (*(dir->d_name) == '.') {
    return 0;
  } else {
    return 1;
  }
}

static int load_file_info(char *dir, char *name, int i, long *total) {
  char *pathname;

  pathname = (char *)malloc(strlen(dir) + strlen(name) + 4);

  if (strchr(name, ' ') != NULL) {
    files_under[i].is_quoted = 1;
  } else {
    files_under[i].is_quoted = 0;
  }

  files_under[i].name = name;
  files_under[i].dir = dir;
  files_under[i].filetype = UNKNOWN;

  attach(pathname, dir, name);

  struct stat buf;

  if (lstat(pathname, &buf) == 0) {
    switch (buf.st_mode & S_IFMT) {
      case S_IFBLK:
        files_under[i].filetype = BLOCKDEV;
        break;
      case S_IFCHR:
        files_under[i].filetype = CHARDEV;
        break;
      case S_IFDIR:
        files_under[i].filetype = DIRECTORY;
        break;
      case S_IFIFO:
        files_under[i].filetype = FIFO;
        break;
      case S_IFLNK:
        files_under[i].filetype = SYMBOLIC_LINK;
        break;
      case S_IFREG:
        files_under[i].filetype = NORMAL;
        break;
      case S_IFSOCK:
        files_under[i].filetype = SOCKET;
        break;
      default:
        files_under[i].filetype = UNKNOWN;
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

  n = (all ? scandir(dir, &namelist, NULL, alphasort)
           : scandir(dir, &namelist, remove_hidden, alphasort));

  if (n == -1) {
    return -1;
  } else if (n == 0) {
    return 0;
  }

  files_under = (FileInfo *)malloc(sizeof(FileInfo) * (n + 1));

  for (int i = 0; i < n; i++) {
    if (load_file_info(dir, namelist[i]->d_name, i, &total) == -1) {
      free(files_under);
      free(namelist);
      return -1;
    }
    width = MAX(width, strlen(files_under[i].name) + 2 + 1);
  }

  if (ll) {
    fprintf(stdout, "total %ldK\n", total / 2);
  }

  for (int i = 0; i < n; i++) {
    format_functions[ll](&files_under[i]);
    free(namelist[i]);
  }

  free(files_under);

  free(namelist);

  fprintf(stdout, "\n");
  return 0;
}

static int list_single_file(char *file, int ll) {
  files_under = (FileInfo *)malloc(sizeof(FileInfo) * (1));

  long total = 0;

  if (load_file_info(".", file, 0, &total) == -1) {
    free(files_under);
    return -1;
  }
  width = (int)strlen(files_under[0].name) + 2 + 1;

  format_functions[ll](&files_under[0]);

  free(files_under);
  fprintf(stdout, "\n");
  return 0;
}

static int is_dir(char *dir) {
  struct stat dir_stat;
  if (stat(dir, &dir_stat) != 0) return 0;
  return S_ISDIR(dir_stat.st_mode);
}

int list_files_internal(arglist_t *args) {
  if (args == NULL) {
    enumerate_files_in_dir(".", 0, 0);
    return 0;
  }
  int all = 0, ll = 0;
  char c;
  reset_get_command_opt();
  while ((c = (char)get_command_opt(args, "la")) != -1) {
    switch (c) {
      case 'l':
        ll = 1;
        break;
      case 'a':
        all = 1;
        break;
      case '?':
        fprintf(stderr, "ls: Invalid argument %s: format ls -[a, l]\n",
                nonopt->_text);
        return -1;
      default:
        continue;
    }
  }
  if (nonopt == NULL) {
    enumerate_files_in_dir(".", all, ll);
  } else {
    int cnt = 0;
    for (arglist_t *curr = nonopt; curr != NULL; curr = curr->_next) {
      if (*(curr->_text) != '-') {
        cnt++;
      }
    }
    for (arglist_t *curr = nonopt; curr != NULL; curr = curr->_next) {
      if (*(curr->_text) != '-') {
        if (cnt > 1) printf("%s:\n", curr->_text);
        if (is_dir(curr->_text)) {
          if (enumerate_files_in_dir(curr->_text, all, ll) == -1) {
            return -1;
          }
        } else if (list_single_file(curr->_text, ll) == -1) {
          return -1;
        }
        if (cnt > 1) printf("\n");
      }
    }
  }
  return 0;
}
