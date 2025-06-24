#include "lexer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
    if (strncmp(cmd, "exit", 4) == 0) {
      struct token *arg0 = t->next;
      if (arg0 != NULL) {
        exitcode = atoi(t->next->value);
      }
      break;
    } else if (strncmp(cmd, "echo", 4) == 0) {
      printToken(t->next);
    } else {
      printf("%s: command not found\n", cmd);
    }
  }
  return exitcode;
}
