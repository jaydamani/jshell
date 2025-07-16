#!/bin/sh
#
# This script is used to compile the program on CodeCrafters
#
# This runs before .codecrafters/run.sh
#
# Learn more: https://codecrafters.io/program-interface
set -e # Exit on failure

CFLAGS='-Wall -Werror'
CFLAGS_ASAN='-fsanitize=address  -g'
LDFLAGS='-lreadline'
mkdir -p build
gcc src/main.c -o build/shell $LDFLAGS $CFLAGS $CFLAGS_ASAN
