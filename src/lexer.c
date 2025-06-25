#include "lexer.h"
#include <linux/limits.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

bool isDelimeter(char chr) { return chr == ' ' || chr == '\0'; }

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

char **tokenize(char *str, size_t *argc) {
  int start = 0, end = 0;
  bool isParsingToken = false;
  struct token *tail = malloc(sizeof(struct token));
  tail->value = (char*)0;
  struct token *head = tail;
  int tc = 0;
  while (end <= strlen(str)) {
    if (isDelimeter(str[end])) {
      if (isParsingToken) {
        isParsingToken = false;
        int len = end - start;
        char *out = malloc(len + 1);
        strncpy(out, &str[start], len);
        out[len] = '\0';
        tail->value = out;
        tc++;
        struct token *t = malloc(sizeof(struct token));
        t->value = (char*)0;
        tail->next = t;
        tail = t;
      }
    } else if (!isParsingToken) {
      isParsingToken = true;
      start = end;
    }
    end++;
  }

  char **args = malloc((tc+1) * sizeof(char *));
  struct token *p;
  for (int i = 0; i < tc+1; i++) {
    args[i] = head->value;
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

void freeTokens(char ** tokens, int tc) {
    for (int i = 0;i < tc; i++) {
        free(tokens[i]);
    }
    free(tokens);
}
