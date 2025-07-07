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

enum REDIR_TYPE { R_INPUT = T_LESS, R_OUT_WRITE = T_GTR, R_OUT_APPEND = T_GTREQ};

typedef struct redirection {
  enum REDIR_TYPE type;
  token *word;
  struct redirection *next;
} redirection;

typedef struct sc_arg {
    char *str;
    struct sc_arg* next;
} sc_arg;

typedef struct simple_command {
  char *name;
  sc_arg *args;
  int argc;
  redirection *redirects;
} simple_command;

enum L_STATE parse(char *str, simple_command **sc);
