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

enum L_STATE nextToken(struct lexer *l) {
  struct token *t = &l->tk;
  t->flags = 0;

  while (isBlank(*l->curr))
    l->curr++;

  if (*l->curr == '#') {
    while (*l->curr != '\0' && *l->curr != '\n')
      l->curr++;
  }
  if (*l->curr == '\n' || *l->curr == ';') {
    t->str = l->curr;
    t->len = 1;
    t->type = T_LINEBREAK;
    l->curr++;
    return l->state = L_EOL;
  }
  if (*l->curr == '\0') {
    t->type = T_END;
    return l->state = L_EOF;
  }

  char *start = l->curr;

  if (*l->curr >= '0' && *l->curr <= '9') {
    l->curr++;
  }
  if (strncmp(l->curr, ">>", 2) == 0) {
    t->type = T_DGTR;
    l->curr += 2;
    if (*l->curr == '&') {
      l->curr++;
    }
    t->str = start;
    t->len = l->curr - start;
    return l->state = L_CONTINUE;
  } else if (*l->curr == '>') {
    t->type = T_GTR;
    l->curr++;
    if (*l->curr == '&') {
      l->curr++;
    }
    t->str = start;
    t->len = l->curr - start;
    return l->state = L_CONTINUE;
  } else {
    // go back to consume as identifiers
    l->curr = start;
  }

  t->type = T_WORD;
  do {
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
  t->str = start;
  t->len = l->curr - start;
  return l->state = L_CONTINUE;
}

void freeToken(struct token *t) { free(t->str); }
// int main(void) { parse("a 'b  'a"); }
