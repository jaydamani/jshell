#ifndef BUILTIN_H
#define BUILTIN_H

#include "parser/parser.h"

#define BUILTINS_COUNT 6

typedef int (*BUILTIN_CMD)(simple_command *sc);
typedef struct builtin_cmd {
    char *name;
    BUILTIN_CMD func;
} builtin_cmd;


int find_builtin_cmd(char *cmd);
int exec_builtin(simple_command *sc, int i);
#endif
