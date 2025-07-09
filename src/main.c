#include "builtins.h"
#include <errno.h>
#include <fcntl.h>
#include <linux/limits.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

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

int main(int argc, char *argv[]) {
  setbuf(stdout, NULL);
  char input[100];

  int exitcode = 0;
  for (;;) {
    printf("$ ");
    fgets(input, 100, stdin);

    input[strlen(input) - 1] = '\0';

    simple_command *sc;
    int status = parse(input, &sc);

    // printf("DEBUG> %s\n", sc->name);
    // printf("DEBUG> %d\n", sc->argc);
    // sc_arg *arg = sc->args;
    // for (int i = 0; i < sc->argc; i++) {
    //   printf("DEBUG> %s\n", arg->str);
    //   arg = arg->next;
    // }
    char *cmd_path;
    int builtin = find_builtin_cmd(sc->name);
    if (builtin != -1) {
      redirection *r = sc->redirects;
      int redirc = sc->redirc;
      int fds[1];
      int clones[1];
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
        exit(-1);
      } else if (pid == -1) {
        printf("fork error: %s\n", strerror(errno));
      }
      int wstatus;
      int s = wait(&wstatus);
      free(cmd_path);
    } else {
      printf("%s: command not found\n", sc->name);
      exitcode = 127;
    }
  }
  return exitcode;
}
