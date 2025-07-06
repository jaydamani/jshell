#include "lexer.h"
#include <linux/limits.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <wordexp.h>

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

enum token_type tokenType(char *str) { return T_IDENTIFIER; }

enum L_OUT nextToken(struct lexer *l, struct token *t) {
  enum token_type tt;

  while (isBlank(*l->curr))
    l->curr++;

  if (*l->curr == '#') {
    while (*l->curr != '\0' && *l->curr != '\n')
      l->curr++;
  }
  if (*l->curr == '\n' || *l->curr == ';') {
    t->str = strndup(l->curr, 1);
    t->type = T_EOC;
    l->curr++;
    return L_CONTINUE;
  }
  if (*l->curr == '\0') {
    t->type = T_END;
    return L_EOF;
  }
  char *start = l->curr;
  do {
    if (*l->curr == '"') {
      do {
        l->curr++;
        if (*l->curr == '\\')
          l->curr++;
      } while (*l->curr != '\0' && *l->curr != '"');
      if (*l->curr == '\0')
        return L_EOF_DQUOTE;
    } else if (*l->curr == '\'') {
      do
        l->curr++;
      while (*l->curr != '\0' && *l->curr != '\'');
      if (*l->curr == '\0')
        return L_EOF_QUOTE;
    } else if (*l->curr == '\\') {
      if (*(l->curr + 1) != 0)
        l->curr++;
    }
    l->curr++;
  } while (isNotSpecialChar(*l->curr));
  t->str = strndup(start, l->curr - start);
  t->type = T_IDENTIFIER;
  return L_CONTINUE;
}

void freeToken(struct token *t) { free(t->str); }

char **parse(char *str, size_t *tc) {
  struct lexer *l;
  createLexer(str, &l);

  struct token *t = malloc(sizeof(struct token));
  enum L_OUT end;
  wordexp_t *w = malloc(sizeof(wordexp_t));
  while ((end = nextToken(l, t)) == L_CONTINUE) {
    if (t->type == T_IDENTIFIER) {
      wordexp(t->str, w, WRDE_APPEND);
    }
    // freeToken(t);
  }
  // for (int i = 0; i < w->we_wordc; i++) {
  //   printf(">%s<\n", w->we_wordv[i]);
  // }
  // free(t);
  *tc = w->we_wordc;
  return w->we_wordv;
}

// int main(void) { parse("a 'b  'a"); }
