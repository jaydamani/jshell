#include "lexer.h"
#include <stdio.h>
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
          printf("%s: not found\n", arg0);
        }
      }
    } else {
      printf("%s: command not found\n", cmd);
    }
  }
  return exitcode;
}
