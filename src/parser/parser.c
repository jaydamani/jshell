#include "parser.h"
#include "lexer.c"
#include "lexer.h"
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int parse_redir(lexer *lexer, redirection **redirect) {
  *redirect = malloc(sizeof(redirection));
  (*redirect)->type = (enum REDIR_OP)lexer->tk.type;
  (*redirect)->n = *lexer->tk.str == '>' ? 1 : *lexer->tk.str - '0';
  nextToken(lexer);
  if (lexer->tk.type != T_WORD)
    return -1;
  (*redirect)->word = strndup(lexer->tk.str, lexer->tk.len);
  redirect = &(*redirect)->next;
  return 0;
}

enum L_STATE parse(const char *str, simple_command **res) {
  simple_command **res_tail = res;

  if (str == NULL || *str == '\0') {
    return L_EOF;
  }

  struct lexer l = {
      .str = str,
      .curr = str,
      .state = 0,
  };
  struct lexer *lexer = &l;

  while (nextToken(lexer) <= L_EOC) {
    simple_command *cmd = *res_tail = malloc(sizeof(simple_command));
    // cmd->name = strndup(l.tk.str, l.tk.len);
    sc_arg **tail = &cmd->words;
    cmd->wordc = 0;
    cmd->redirc = 0;
    redirection **redirect = &cmd->redirects;
    while (l.state == L_CONTINUE) {
      cmd->next = NULL;
      if (l.tk.type == T_WORD) {
        cmd->wordc++;
        *tail = malloc(sizeof(sc_arg));
        (*tail)->str = strndup(l.tk.str, l.tk.len);
        tail = &(*tail)->next;
      } else if (l.tk.type == T_GTR || l.tk.type == T_DGTR) {
        cmd->redirc++;
        if (parse_redir(lexer, redirect) != 0)
            return -1;
      }
      nextToken(lexer);
    }
    res_tail = &cmd->next;
  }
  // for (int i = 0; i < w->we_wordc; i++) {
  //   printf(">%s<\n", w->we_wordv[i]);
  // }

  return l.state;
}

void free_sc(simple_command *sc) {
  if (sc == NULL)
    return;

  sc_arg *tmp_arg, *arg = sc->words;
  for (int i = 0; i < sc->wordc; i++) {
    tmp_arg = arg;
    arg = arg->next;
    free(tmp_arg->str);
    free(tmp_arg);
  }
  redirection *tmp_redir, *redir = sc->redirects;
  for (int i = 0; i < sc->redirc; i++) {
    tmp_redir = redir;
    redir = redir->next;
    free(tmp_redir->word);
    free(tmp_redir);
  }
  free(sc);
}
