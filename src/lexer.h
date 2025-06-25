#include <stddef.h>
struct token {
  char *value;
  struct token *next;
};

char **tokenize(char *str, size_t* tc);
void printToken(struct token *t);
void freeTokens(char ** tokens, int tc);
