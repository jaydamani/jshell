#include <stddef.h>

typedef enum L_STATE {
  L_CONTINUE = 0,
  L_EOF,
  L_EOF_QUOTE,
  L_EOF_DQUOTE
} L_STATE;

struct lexer {
  char *curr;
  char *str;
  L_STATE state;
};

enum token_type {
  T_WORD,
  // T_DELIMETER,
  T_END,
  T_LINEBREAK,
  T_GTR,
  T_GTREQ,
  T_LESS
};

enum LW_FLAGS {
  LW_NONNUM = 1 << 0 /* word contains non-numeric character */
};

typedef struct token {
  char *str;
  enum token_type type;
  int flags;
  struct token *next;
} token;

void freeToken(struct token *t);
enum L_STATE nextToken(struct lexer *l, struct token *t);
void createLexer(char *str, struct lexer **l);
