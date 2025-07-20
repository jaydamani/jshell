#ifndef LEXER_H
#define LEXER_H

typedef enum L_STATE {
  L_CONTINUE = 0,
  L_EOL,
  L_EOF,
  L_EOF_QUOTE,
  L_EOF_DQUOTE
} L_STATE;

enum token_type {
  T_WORD,
  // T_DELIMETER,
  T_END,
  T_LINEBREAK,
  T_GTR,
  T_DGTR,
};

enum LW_FLAGS {
  LW_NONNUM = 1 << 0 /* word contains non-numeric character */
};

typedef struct token {
  const char *str;
  int len;
  int flags;
  enum token_type type;
  struct token *next;
} token;

struct lexer {
  const char *curr;
  const char *str;
  L_STATE state;
  struct token tk;
};

enum L_STATE nextToken(struct lexer *l);
#endif
