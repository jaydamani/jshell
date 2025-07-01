#include "lexer.h"
#include <linux/limits.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <wordexp.h>

bool isDelimeter(char chr) { return chr == ' ' || chr == '\0'; }

bool isNotSpecialChar(char chr) { return !isDelimeter(chr); }

void printToken(struct token *t) {
  struct token *p = t->next;
  printf("%s", t->value);
  while (p != NULL) {
    if (p->value)
      printf(" %s", p->value);
    else
      printf(" <null>");
    p = p->next;
  }
  printf("\n");
}

struct token *createEmptyToken() {
  struct token *t = malloc(sizeof(struct token));
  t->value = (char *)0;
  t->type = T_END;
  return t;
}

int appendToken(char *value, enum token_type type, struct token **tail,
                 int tc) {
  (*tail)->value = value;
  (*tail)->type = type;
  struct token *t = createEmptyToken();
  tc++;
  (*tail)->next = t;
  (*tail) = t;
  return tc;
}

int appendStrToken(char *chr, int len, struct token **tail, int tc) {
  char *out = malloc(len + 1);
  strncpy(out, chr, len);
  out[len] = '\0';
  appendToken(out, T_IDENTIFIER, tail, tc);
}

char **tokenize(char *str, size_t *argc) {
  struct token *tail = createEmptyToken();
  struct token *head = tail;
  int tc = 0;
  while (*str != 0) {
    if (*str == '#') {
      while (*str != 0 && *str != '\n')
        str++;
    } else if (isDelimeter(*str)) {
      str++;
    } else if (*str == '"') {
      char *start = str;
      char prevChar = 0;
      while (*str != 0 && (*str != '"' || prevChar != '\\'))
        prevChar = *str++;
      tc = appendStrToken(start, str - start + 1, &tail, tc);
    } else if (*str == '"') {
      char *start = str;
      char prevChar = 0;
      while (*str != 0 && *str != '"')
        prevChar = *str++;
      tc = appendStrToken(start, str - start + 1, &tail, tc);
    } else {
      char *start = str;
      while (*str != 0 && isNotSpecialChar(*str))
        str++;
      // printf("%d \n", tc);
      tc = appendStrToken(start, str - start + 1, &tail, tc);
    }
  }

  char **args = malloc((tc + 1) * sizeof(char *));
  struct token *p;
  wordexp_t *w = malloc(sizeof(wordexp_t));
  for (int i = 0; i < tc + 1; i++) {
    char *val = head->value;
    if (head->type == T_IDENTIFIER) {
      wordexp(val, w, 0);
      // printf("%zu", w->we_wordc);
      args[i] = w->we_wordv[0];
      // for (int i = 0; i < w->we_wordc; i++)
      // printf("> %s <\n", w->we_wordv[i]);
    } else {
      args[i] = val;
    }
    p = head;
    head = head->next;
    free(p);
  }
  *argc = tc;
  return args;
}

// int main(void) {
//   size_t tc;
//   char **arr = tokenize("cmd arg1 arg2", &tc);
//   for (int i = 0; i < tc; i++) {
//     if (arr[i])
//       printf("%s\n", arr[i]);
//     else
//       printf("<null>\n");
//   }
// }

void freeTokens(char **tokens, int tc) {
  for (int i = 0; i < tc; i++) {
    free(tokens[i]);
  }
  free(tokens);
}
