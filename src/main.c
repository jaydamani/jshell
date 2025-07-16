#include <asm-generic/errno-base.h>
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "command.c"
#include "parser/parser.c"

#include <readline/history.h>
#include <readline/readline.h>
#include <unistd.h>

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

char *hist_path;
int current_hist_start;

void update_history() {
  if (!hist_path)
    return;
  // we need to use write_history if the file does not exist
  if (access(hist_path, W_OK)) {
    if (errno == ENOENT && write_history(hist_path))
      printf("error: %s\n", strerror(errno));
    else
      printf("error: %s\n", strerror(errno));
  } else if (append_history(history_length - current_hist_start, hist_path))
    printf("error: %s\n", strerror(errno));
}

void init_rl() {
  hist_path = getenv("HISTFILE");
  if (hist_path != NULL) {
    if (read_history(hist_path) != 0 && errno != ENOENT) {
      printf("error: %s", strerror(errno));
    }
    current_hist_start = history_length;
    atexit(update_history);
  }

  /* Allow conditional parsing of the ~/.inputrc file. */
  rl_readline_name = "JSHell";

  /* Tell the completer that we want a crack first. */
  rl_attempted_completion_function = handle_completion;
}

bool cmp_with_last_entry(char *input) {
  HISTORY_STATE *hist = history_get_history_state();
  bool res = true;
  if (hist->length) {
    char *last = hist->entries[hist->length - 1]->line;
    res = strcmp(input, last) != 0;
  }
  free(hist);
  return res;
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
    if (input && *input && cmp_with_last_entry(input))
      add_history(input);
    simple_command *sc;
    int status = parse(input, &sc);
    if (status == L_EOL || status == L_EOF) {
      exec_cmd(sc);
      free_sc(sc);
    }
    free(input);
  }
  return exitcode;
}
