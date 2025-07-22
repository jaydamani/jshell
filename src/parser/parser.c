#include "parser.h"
#include "lexer.c"
#include "lexer.h"
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

enum L_STATE parse_cmd(const char *str, simple_command **res) {

  if (str == NULL || *str == '\0') {
    return L_EOF;
  }

  struct lexer l = {
      .str = str,
      .curr = str,
      .state = 0,
  };
  struct lexer *lexer = &l;
  // if (l.tk.type != T_WORD) {
  //   return -1;
  // }

  simple_command *cmd = malloc(sizeof(simple_command));
  // cmd->name = strndup(l.tk.str, l.tk.len);
  sc_arg **tail = &cmd->words;
  cmd->wordc = 0;
  cmd->redirc = 0;
  redirection **redirect = &cmd->redirects;
  while (nextToken(lexer) == L_CONTINUE) {
    if (l.tk.type == T_WORD) {
      cmd->wordc++;
      *tail = malloc(sizeof(sc_arg));
      (*tail)->str = strndup(l.tk.str, l.tk.len);
      tail = &(*tail)->next;
    } else if (l.tk.type == T_GTR || l.tk.type == T_DGTR) {
      cmd->redirc++;
      *redirect = malloc(sizeof(redirection));
      (*redirect)->type = (enum REDIR_OP)l.tk.type;
      (*redirect)->n = *l.tk.str == '>' ? 1 : *l.tk.str - '0';
      nextToken(&l);
      if (l.tk.type != T_WORD)
        return -1;
      (*redirect)->word = strndup(l.tk.str, l.tk.len);
      redirect = &(*redirect)->next;
    }
  }
  // for (int i = 0; i < w->we_wordc; i++) {
  //   printf(">%s<\n", w->we_wordv[i]);
  // }

  *res = cmd;
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
