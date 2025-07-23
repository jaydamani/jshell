#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"

enum REDIR_OP { R_OUT_WRITE = T_GTR, R_OUT_APPEND = T_DGTR };

// GRAMMAR: ([n]redir_op word)+
typedef struct redirection {
  enum REDIR_OP type;
  int n;
  char *word;
  struct redirection *next;
} redirection;

typedef struct sc_arg {
  char *str;
  struct sc_arg *next;
} sc_arg;

typedef struct simple_command {
  sc_arg *words;
  int wordc;
  redirection *redirects;
  int redirc;
  struct simple_command *next;
} simple_command;

enum L_STATE parse(const char *str, simple_command **sc);
void free_sc(simple_command *sc);
#endif
