#include "lexer.h"
#include <linux/limits.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <wordexp.h>

#define L_NONNUMERIC_FLAG 0x0001

bool isBlank(char chr) { return chr == ' '; }

bool isNotSpecialChar(char chr) {
  return !isBlank(chr) && chr != '\n' && chr != ';' && chr != 0 && chr != '#';
}

void createLexer(char *str, struct lexer **l) {
  *l = malloc(sizeof(struct lexer));
  struct lexer *lexer = *l;
  lexer->str = str;
  lexer->curr = str;
}

enum token_type tokenType(char *str) { return T_WORD; }

enum L_STATE nextToken(struct lexer *l, struct token *t) {
  t->flags = 0;

  while (isBlank(*l->curr))
    l->curr++;

  if (*l->curr == '#') {
    while (*l->curr != '\0' && *l->curr != '\n')
      l->curr++;
  }
  if (*l->curr == '\n' || *l->curr == ';') {
    t->str = strndup(l->curr, 1);
    t->type = T_LINEBREAK;
    l->curr++;
    return l->state = L_CONTINUE;
  }
  if (*l->curr == '\0') {
    t->type = T_END;
    return l->state = L_EOF;
  }

  if (*l->curr == '>') {
    t->type = T_GTR;
    t->str = strndup(l->curr, 1);
    return l->state = L_CONTINUE;
  }

  t->type = T_WORD;
  char *start = l->curr;
  do {
    if (*l->curr <= '0' || *l->curr >= '9') {
      t->flags |= L_NONNUMERIC_FLAG;
    }

    if (*l->curr == '"') {
      while (*l->curr != '"') {
        if (*++l->curr == '\\' && *++l->curr != '\0')
          l->curr++;
        if (*l->curr == '\0')
          return l->state = L_EOF_DQUOTE;
      }
    } else if (*l->curr == '\'') {
      do {
        if (*++l->curr == '\0')
          return l->state = L_EOF_QUOTE;
      } while (*l->curr != '\'');
    } else if (*l->curr == '\\') {
      if (*(l->curr + 1) != 0)
        l->curr++;
    }
    l->curr++;
  } while (isNotSpecialChar(*l->curr));
  t->str = strndup(start, l->curr - start);
  return l->state = L_CONTINUE;
}

void freeToken(struct token *t) { free(t->str); }

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

// int main(void) { parse("a 'b  'a"); }
