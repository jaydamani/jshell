#include "stdio.h"
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "../builtins.h"
#include "../parser/parser.h"
#include "chartrie.c"

#include <readline/readline.h>

cmap cmd_map = {};

typedef char *completion_func(simple_command *sc, int state);

char *cmd_completion(simple_command *sc, int state) {
  static node_list *node;
  static bool can_continue;
  static int min_len;
  if (!state) {
    node = find_str_node(&cmd_map.possible_chars, sc->name);
    if (node == NULL)
      return NULL;
    can_continue = false;
    min_len = 0;
  }
  char *match = traverse(&cmd_map, node, min_len, &can_continue);
  if (!can_continue) {
    if (node->chr_node.value == '\0') {
      free(match);
      return NULL;
    }
    node = node->next_child;
    min_len = 0;
  } else {
    min_len = strlen(match);
  }
  char *res = malloc(strlen(sc->name) + strlen(match) + 1);
  strcpy(res, sc->name);
  strcat(res, match);
  free(match);
  return res;
}
char *arg_completion(simple_command *sc, int state) { return (char *)NULL; }

char *single_completion(const char *txt, int state) {
  static simple_command *sc;
  static completion_func *func;

  if (!state) {
    free_sc(sc);
    sc = NULL;
    parse(txt, &sc);
    if (!sc)
      return NULL;
  }
  if (sc->argc == 0)
    func = cmd_completion;
  else
    func = arg_completion;
  return func(sc, state);
}

char **handle_completion(const char *_, int start, int end) {
  char **matches = (char **)NULL;
  char *text = strndup(rl_line_buffer, end);
  matches = rl_completion_matches(text, single_completion);
  free(text);
  return (matches);
}

void init_completion() {
  for (int i = 0; i < cmd_len; i++) {
    add_to_cmap(&cmd_map, cmd_list[i]);
  }
}

void _() { printf("dasfs"); }
