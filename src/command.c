#include "command.h"
#include "builtins.c"
#include <errno.h>
#include <fcntl.h>
#include <linux/limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#define PATH_SEP "/"

int sep_len = strlen(PATH_SEP);

int append_path(const char *dir, const char *file, char *res) {
  char *buf = malloc(PATH_MAX);
  strcpy(buf, dir);

  if (strcmp(&buf[strlen(buf) - sep_len], PATH_SEP) != 0) {
    strcat(buf, PATH_SEP);
  }

  strcat(buf, file);
  strcpy(res, buf);
  free(buf);
  return 0;
}

int create_arg_array(simple_command *sc, char *tokens[]) {
  tokens[0] = sc->name;
  sc_arg *arg = sc->args;
  for (int i = 1; i <= sc->argc; i++) {
    tokens[i] = arg->str;
    arg = arg->next;
  }
  tokens[sc->argc + 1] = NULL;
  return 0;
}

char *find_path_cmd(char *cmd) {
  char *path_var = strdup(getenv("PATH"));
  char *path = path_var;
  char *save_ptr;
  char *file_path = malloc(PATH_MAX);
  char *res = NULL;

  while ((path = strtok_r(path, ":", &save_ptr))) {
    append_path(path, cmd, file_path);
    char *expanded_path = realpath(file_path, NULL);
    if (access(expanded_path, X_OK) == 0) {
      res = expanded_path;
      break;
    }
    free(expanded_path);
    path = NULL;
  }
  free(path_var);
  free(file_path);
  return res;
}

int exec_cmd(simple_command *sc) {

  int ec = 0;
  char *cmd_path;
  int builtin = find_builtin_cmd(sc->name);
  if (builtin != -1) {
    redirection *r = sc->redirects;
    int fds[sc->redirc];
    int clones[sc->redirc];
    for (int i = 0; i < sc->redirc; i++) {
      int flags = O_CREAT | O_WRONLY | O_APPEND;
      flags |= r->type == R_OUT_WRITE ? O_TRUNC : 0;
      int fd = open(r->word, flags, 0666 /*😈*/);
      clones[i] = dup(r->n);
      dup2(fd, r->n);
      fds[i] = fd;
    }

    exec_builtin(sc, builtin);

    for (int i = 0; i < sc->redirc; i++) {
      dup2(clones[i], r->n);
      close(clones[i]);
      close(fds[i]);
    }
  } else if ((cmd_path = find_path_cmd(sc->name))) {
    char *tokens[sc->argc + 2];
    pid_t pid;

    if ((pid = fork()) == 0) {
      create_arg_array(sc, tokens);

      redirection *r = sc->redirects;
      int fds[sc->redirc];
      for (int i = 0; i < sc->redirc; i++) {
        int flags = O_CREAT | O_WRONLY | O_APPEND;
        flags |= r->type == R_OUT_WRITE ? O_TRUNC : 0;
        int fd = open(r->word, flags, 0666 /*😈*/);
        dup2(fd, r->n);
        fds[i] = fd;
      }

      int s = execv(cmd_path, tokens);
      if (s != 0)
        printf("exec error: %s\n", strerror(errno));

      for (int i = 0; i < sc->redirc; i++) {
        close(fds[i]);
      }
      exit(s);
    } else if (pid == -1) {
      printf("fork error: %s\n", strerror(errno));
    }
    wait(&ec);
    free(cmd_path);
  } else {
    printf("%s: command not found\n", sc->name);
    ec = 127;
  }
  return ec;
}
