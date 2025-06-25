#include "lexer.h"
#include "path.h"
#include <linux/limits.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#define BUILTINS_COUNT 3

char *cmd_list[BUILTINS_COUNT] = {"exit", "echo", "type"};

int find_builtin_cmd(char *cmd) {
  for (int i = 0; i < BUILTINS_COUNT; i++) {
    if (strcmp(cmd_list[i], cmd) == 0) {
      return i;
    }
  }
  return -1;
}

char* find_path_cmd(char* cmd) {
    char* path;
    char* path_var = strdup(getenv("PATH"));
    char* file_path = malloc(PATH_MAX);
    char* res = NULL;

    while((path = strtok_r(path_var, ":", &path_var))) {
        append_path(path, cmd, file_path);
        realpath(file_path, file_path);
        if (access(file_path, F_OK) == 0) {
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

    struct token *t;
    tokenize(input, &t);

    char *cmd = t->value;

    int builtin = find_builtin_cmd(cmd);
    if (builtin == 0) {
      struct token *arg0 = t->next;
      if (arg0 != NULL) {
        exitcode = atoi(t->next->value);
      }
      break;
    } else if (builtin == 1) {
      printToken(t->next);
    } else if (builtin == 2) {
      struct token *arg0_t = t->next;
      if (arg0_t != NULL) {
        char *arg0 = arg0_t->value;
        if (find_builtin_cmd(arg0) != -1) {
          printf("%s is a shell builtin\n", arg0);
        } else {
          char *cmd_path = find_path_cmd(arg0);
          if (cmd_path != NULL) {
            printf("%s is %s\n", arg0, cmd_path);
            free(cmd_path);
          } else {
            printf("%s: not found\n", arg0);
          }
        }
      }
    } else {
      printf("%s: command not found\n", cmd);
    }
    freeToken(t);
  }
  return exitcode;
}
