#include <stddef.h>

struct lexer {
  char *curr;
  char *str;
};

enum L_OUT { L_CONTINUE, L_EOF, L_EOF_QUOTE, L_EOF_DQUOTE };

enum token_type {
  T_IDENTIFIER,
  T_OPERATION,
  // T_DELIMETER,
  T_END,
  T_EOC
};

struct token {
  char *str;
  enum token_type type;
};

char **parse(char *str, size_t *tc);
