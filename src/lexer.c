#include <linux/limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lexer.h"


bool isDelimeter(char chr) { return chr == ' ' || chr == '\0'; }

void printToken(struct token *t) {
  struct token *p;
  bool isFirst = true;
  for (p = t; p != NULL; p = p->next) {
    if (!isFirst) {
        printf(" ");
    }
    isFirst = false;
    printf("%s", p->value);
  }
  printf("\n");
}

int tokenize(char *str, struct token **res) {
  int start = 0, end = 0;
  bool isParsingToken = false;
  struct token *tail = NULL;
  while (end <= strlen(str)) {
    if (isDelimeter(str[end])) {
      if (isParsingToken) {
        isParsingToken = false;
        int len = end - start;
        char *out = malloc(len+1);
        strncpy(out, &str[start], len);
        out[len] = '\0';
        struct token *t = malloc(sizeof(struct token));
        t->value = out;
        if (tail == NULL) {
          *res = t;
        }
        else {
            tail->next = t;
        }
        tail = t;
      }
    } else if (!isParsingToken) {
      isParsingToken = true;
      start = end;
    }
    end++;
  }

  return 0;
}

// int main(void) {
//   struct token *t;
//   struct token *p;
//   tokenize("cmd arg1 arg2", &t);
//   printToken(t);
//   free(t);
// }

void freeToken(struct token *t) {
  struct token *p;
  while (t != NULL) {
    p = t;
    t = t->next;
    free(p->value);
    free(p);
  }
}
