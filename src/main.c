#include "lexer.h"
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

    size_t tcount;
    char **tokens = parse(input, &tcount);

    // printf("DEBUG> %zu\n", tcount);
    // for (int i = 0; i < tcount + 1; i++) {
    //   if (tokens[i])
    //     printf("DEBUG> %s\n", tokens[i]);
    //   else
    //     printf("<null>\n");
    // }

    char *cmd = tokens[0];
    char *cmd_path;
    int builtin = find_builtin_cmd(cmd);
    if (builtin == 0) {
      if (tcount > 1) {
        exitcode = atoi(tokens[1]);
      }
      break;
    } else if (builtin == 1) {
      for (int i = 1; i < tcount; i++)
        printf("%s%s", i == 1 ? "" : " ", tokens[i]);
      printf("\n");
    } else if (builtin == 2) {
      if (tcount < 2) {
        continue;
      }
      char *arg0 = tokens[1];
      if (find_builtin_cmd(arg0) != -1) {
        printf("%s is a shell builtin\n", arg0);
      } else if ((cmd_path = find_path_cmd(arg0))) {
        printf("%s is %s\n", arg0, cmd_path);
        free(cmd_path);
      } else {
        printf("%s: not found\n", arg0);
      }
    } else if (builtin == 3) {
      char buf[PATH_MAX];
      printf("%s\n", getcwd(buf, PATH_MAX));
    } else if (builtin == 4) {
      char *path = tokens[1];
      realpath(path, path);
      if (access(path, F_OK) != 0)
        printf("cd: %s: No such file or directory\n", path);
      else
        chdir(path);
    } else if ((cmd_path = find_path_cmd(cmd))) {
      pid_t pid = fork();

      if (pid == -1) {
        printf("%s\n", strerror(errno));
      } else if (pid == 0) {
        int s = execv(cmd_path, tokens);
        if (s != 0)
          printf("%s\n", strerror(errno));
        exit(-1);
      }
      int wstatus;
      int s = wait(&wstatus);
      free(cmd_path);
    } else {
      printf("%s: command not found\n", cmd);
    }
  }
  return exitcode;
}
