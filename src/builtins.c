#include "builtins.h"
#include "command.h"
#include "parser/parser.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <readline/history.h>
#include <linux/limits.h>
#include <unistd.h>

char *cmd_list[] = {"exit", "echo", "type", "pwd", "cd", "history"};
int cmd_len = sizeof(cmd_list) / sizeof(cmd_list[0]);

int find_builtin_cmd(char *cmd) {
  for (int i = 0; i < cmd_len; i++) {
    if (strcmp(cmd_list[i], cmd) == 0) {
      return i;
    }
  }
  return -1;
}

int b_exit(simple_command *sc) {
  int exitcode = 0;
  if (sc->argc == 1) {
    exitcode = atoi(sc->args->str);
  }
  exit(exitcode);
}

int echo(simple_command *sc) {
  sc_arg *arg = sc->args;
  for (int i = 0; i < sc->argc; i++) {
    printf("%s%s", i == 0 ? "" : " ", arg->str);
    arg = arg->next;
  }
  printf("\n");
  return 0;
}

int type(simple_command *sc) {
  char *cmd_path;
  if (sc->argc < 1) {
    return 0;
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
  return 0;
}

int pwd(simple_command *sc) {
  char buf[PATH_MAX];
  printf("%s\n", getcwd(buf, PATH_MAX));
  return 0;
}

int cd(simple_command *sc) {
  char *path = sc->args->str;
  char *expanded_path = realpath(path, NULL);
  if (access(expanded_path, F_OK) == 0)
    chdir(expanded_path);
  else {
    printf("cd: %s: No such file or directory\n", path);
  }
  free(expanded_path);
  return 0;
}

int history(simple_command *sc) {
    int exit = 0;
    HISTORY_STATE *hist = history_get_history_state();
    int max = 10;
    if (sc->argc > 0) max = atoi(sc->args->str);
    if (max > hist->length) max = hist->length;

    for (int i = hist->length - max;i < hist->length; i++) {
        printf("    %d  %s\n", i + 1, hist->entries[i]->line);
    }

    free(hist);
    return exit;
}

BUILTIN_CMD cmd_f_list[] = {b_exit, echo, type, pwd, cd, history};

int exec_builtin(simple_command *sc, int i) { return cmd_f_list[i](sc); }
