#include "lexer.h"

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
