#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "command.c"
#include "parser/parser.c"

#include <readline/readline.h>

extern char *cmd_list[];

char *single_completion(const char *txt, int state) {
  static int i, len;

  if (!state) {
    i = -1;
    len = strlen(txt);
  }

  while (++i < BUILTINS_COUNT) {
    if (strncmp(txt, cmd_list[i], len) == 0)
      return strdup(cmd_list[i]);
  };
  return (char *)NULL;
}

char **handle_completion(const char *text, int start, int end) {
  char **matches;

  matches = (char **)NULL;

  /* If this word is at the start of the line, then it is a command
     to complete.  Otherwise it is the name of a file in the current
     directory. */
  if (start == 0)
    matches = rl_completion_matches(text, single_completion);

  return (matches);
  printf("done");
}

void init_rl() {
  /* Allow conditional parsing of the ~/.inputrc file. */
  rl_readline_name = "JSHell";

  /* Tell the completer that we want a crack first. */
  rl_attempted_completion_function = handle_completion;
}

int main(int argc, char *argv[]) {
  init_rl();
  char *input;

  int exitcode = 0;
  for (;;) {
    char *prompt = getenv("PS1");
    if (prompt == NULL) {
      prompt = "$ ";
    }
    input = readline(prompt);

    simple_command *sc;
    char *curr = input;
    int status = parse(input, &sc);
    if (status == L_EOL || status == L_EOF) {
      exec_cmd(sc);
      free_sc(sc);
    }
    free(input);
  }
  return exitcode;
}
