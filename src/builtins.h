#include "parser.h"
#include "path.h"

#define BUILTINS_COUNT 5

typedef int (*BUILTIN_CMD)(simple_command *sc);

int find_builtin_cmd(char *cmd);
int b_exit (simple_command *sc);
int echo(simple_command *sc);
int type(simple_command *sc);
int pwd(simple_command *sc);
int cd(simple_command *sc);
int exec_builtin(simple_command *sc, int i);
