struct token {
  char *value;
  struct token *next;
};

int tokenize(char *str, struct token **res);
void printToken(struct token *t);
void freeToken(struct token *t);
