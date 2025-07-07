#include "parser.h"
#include <stdlib.h>

enum L_STATE parse(char *str, simple_command **res) {

  simple_command *cmd = malloc(sizeof(simple_command));

  struct lexer *l;
  createLexer(str, &l);

  struct token *t = malloc(sizeof(token));
  nextToken(l, t);
  while (l->state == 0) {
    if (t->type == T_LINEBREAK) {
      freeToken(t);
      nextToken(l, t);
    } else if (t->type == T_WORD) {
      cmd->name = t->str;
      sc_arg **tail = &cmd->args;
      cmd->argc = 0;
      while (nextToken(l, t) == 0 && t->type == T_WORD) {
        cmd->argc++;
        *tail = malloc(sizeof(sc_arg));
        (*tail)->str = t->str;
        tail = &(*tail)->next;
      }
      redirection **redirect = &cmd->redirects;
      while (t->type == T_GTR || t->type == T_GTREQ || t->type == T_LESS) {
        token *w = malloc(sizeof(token));
        nextToken(l, w);
        if (w->type != T_WORD)
          return -1;

        *redirect = malloc(sizeof(redirection));
        (*redirect)->word = w;
        (*redirect)->type = (enum REDIR_TYPE)t->type;
        redirect = &(*redirect)->next;
        freeToken(t);
        nextToken(l, t);
      }
    }
  }
  // for (int i = 0; i < w->we_wordc; i++) {
  //   printf(">%s<\n", w->we_wordv[i]);
  // }
  free(t);
  *res = cmd;
  return l->state;
}
