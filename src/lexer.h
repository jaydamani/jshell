#include <stddef.h>

enum token_type {
  T_IDENTIFIER,
  T_OPERATION,
  T_END
};

struct token {
  char *value;
  enum token_type type;
  struct token *next;
};

char **tokenize(char *str, size_t* tc);
void printToken(struct token *t);
void freeTokens(char ** tokens, int tc);
