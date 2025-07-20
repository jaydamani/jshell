#ifndef BUILTIN_H
#define BUILTIN_H

#include "parser/parser.h"

typedef int (*BUILTIN_CMD)(simple_command *sc);
typedef struct builtin_cmd {
    char *name;
    BUILTIN_CMD func;
} builtin_cmd;

extern int cmd_len;
extern char *cmd_list[];

int find_builtin_cmd(char *cmd);
int exec_builtin(simple_command *sc, int i);
#endif
