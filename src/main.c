#include <asm-generic/errno-base.h>
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "command.c"
#include "completion/completion.c"
#include "parser/parser.h"

#include <readline/history.h>
#include <readline/readline.h>
#include <unistd.h>

char *hist_path;
int current_hist_start;

void update_history() {
  if (!hist_path)
    return;
  // use write_history to create file if it does not exist
  if (access(hist_path, W_OK)) {
    if (errno == ENOENT) {
      if (write_history(hist_path))
        printf("error writing history: %s\n", strerror(errno));
    } else if (errno)
      printf("error accessing history path(%d): %s\n", errno, strerror(errno));
  } else if (append_history(history_length - current_hist_start, hist_path))
    printf("error appending history: %s\n", strerror(errno));
}

void init_rl() {
  hist_path = getenv("HISTFILE");
  if (hist_path != NULL) {
    if (read_history(hist_path) && errno != ENOENT) {
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
  init_completion();
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
    simple_command *sc = NULL;
    parse(input, &sc);
    while (sc != NULL) {
        exec_cmd(sc);
        simple_command *tmp = sc;
        sc = sc->next;
        free_sc(tmp);
    }
    free(input);
  }
  return exitcode;
}
