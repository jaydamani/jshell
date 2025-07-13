#ifndef CMD_H
#define CMD_H
#include "parser/parser.h"

char *find_path_cmd(char *cmd);
int exec_cmd(simple_command *sc);
#endif
