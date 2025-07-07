#include "parser.h"
#include "path.h"
#include <errno.h>
#include <linux/limits.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define BUILTINS_COUNT 5

char *cmd_list[BUILTINS_COUNT] = {"exit", "echo", "type", "pwd", "cd"};

int find_builtin_cmd(char *cmd) {
  for (int i = 0; i < BUILTINS_COUNT; i++) {
    if (strcmp(cmd_list[i], cmd) == 0) {
      return i;
    }
  }
  return -1;
}

char *find_path_cmd(char *cmd) {
  char *path;
  char *path_var = strdup(getenv("PATH"));
  char *file_path = malloc(PATH_MAX);
  char *res = NULL;

  while ((path = strtok_r(path_var, ":", &path_var))) {
    append_path(path, cmd, file_path);
    realpath(file_path, file_path);
    if (access(file_path, X_OK) == 0) {
      res = file_path;
      break;
    }
  }
  if (res == NULL) {
    free(file_path);
  }
  return res;
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

    char *cmd = sc->name;
    char *cmd_path;
    int builtin = find_builtin_cmd(cmd);
    if (builtin == 0) {
      if (sc->argc == 1) {
        exitcode = atoi(sc->args->str);
      }
      break;
    } else if (builtin == 1) {
      sc_arg *arg = sc->args;
      for (int i = 0; i < sc->argc; i++) {
        printf("%s%s", i == 1 ? "" : " ", arg->str);
        arg = arg->next;
      }
      printf("\n");
    } else if (builtin == 2) {
      if (sc->argc < 1) {
        continue;
      }
      char *arg_0 = sc->args->str;
      if (find_builtin_cmd(arg_0) != -1) {
        printf("%s is a shell builtin\n", arg_0);
      } else if ((cmd_path = find_path_cmd(arg_0))) {
        printf("%s is %s\n", arg_0, cmd_path);
        free(cmd_path);
      } else {
        printf("%s: not found\n", arg_0);
      }
    } else if (builtin == 3) {
      char buf[PATH_MAX];
      printf("%s\n", getcwd(buf, PATH_MAX));
    } else if (builtin == 4) {
      char *path = sc->args->str;
      realpath(path, path);
      if (access(path, F_OK) != 0)
        printf("cd: %s: No such file or directory\n", path);
      else
        chdir(path);
    } else if ((cmd_path = find_path_cmd(cmd))) {
      char *tokens[sc->argc+2];
      tokens[0] = cmd;
      sc_arg *arg = sc->args;
      for (int i = 1; i <= sc->argc; i++) {
        tokens[i] = arg->str;
        arg = arg->next;
      }
      tokens[sc->argc+1] = NULL;
      pid_t pid = fork();
      if (pid == -1) {
        printf("%s\n", strerror(errno));
      } else if (pid == 0) {

        int s = execv(cmd_path, tokens);
        if (s != 0)
          printf("exec error: %s\n", strerror(errno));
        exit(-1);
      }
      int wstatus;
      int s = wait(&wstatus);
      free(cmd_path);
    } else {
      printf("%s: command not found\n", cmd);
      exitcode = 127;
    }
  }
  return exitcode;
}
