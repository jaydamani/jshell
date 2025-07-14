#include "parser.h"
#include "lexer.c"
#include <stdlib.h>
#include <string.h>

char *Getvalue(struct token *t) {
    return strndup(t->str, t->len);
}

enum L_STATE parse(char *str, simple_command **res) {

  if (str == NULL || *str == '\0') {
    return L_EOF;
  }

  simple_command *cmd = malloc(sizeof(simple_command));

  struct lexer l = {
      .str = str,
      .curr = str,
      .state = 0,
  };
  struct lexer* lexer = &l;
  nextToken(lexer);
  while (l.state == 0) {
    if (l.tk.type == T_LINEBREAK) {
      nextToken(lexer);
    } else if (l.tk.type == T_WORD) {
      cmd->name = strndup(l.tk.str, l.tk.len);
      sc_arg **tail = &cmd->args;
      cmd->argc = 0;
      while (nextToken(lexer) == 0 && l.tk.type == T_WORD) {
        cmd->argc++;
        *tail = malloc(sizeof(sc_arg));
        (*tail)->str = strndup(l.tk.str, l.tk.len);
        tail = &(*tail)->next;
      }
      cmd->redirc = 0;
      redirection **redirect = &cmd->redirects;
      while (l.tk.type == T_GTR || l.tk.type == T_DGTR) {
        cmd->redirc++;
        *redirect = malloc(sizeof(redirection));
        (*redirect)->type = (enum REDIR_OP)l.tk.type;
        // printf("%s\n", t->str);
        (*redirect)->n = *l.tk.str == '>' ? 1 : *l.tk.str - '0';

        nextToken(&l);
        if (l.tk.type != T_WORD)
          return -1;
        (*redirect)->word = strndup(l.tk.str, l.tk.len);

        redirect = &(*redirect)->next;
        nextToken(&l);
      }
    }
  }
  // for (int i = 0; i < w->we_wordc; i++) {
  //   printf(">%s<\n", w->we_wordv[i]);
  // }

  *res = cmd;
  return l.state;
}
