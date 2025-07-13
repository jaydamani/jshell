#ifndef BUILTIN_H
#define BUILTIN_H

#include "parser.h"

#define BUILTINS_COUNT 5

typedef int (*BUILTIN_CMD)(simple_command *sc);

int find_builtin_cmd(char *cmd);
int exec_builtin(simple_command *sc, int i);
#endif
