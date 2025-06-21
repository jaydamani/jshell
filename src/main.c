#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
  setbuf(stdout, NULL);
  char input[100];

  for (;;){
    printf("$ ");

    // Wait for user input
    fgets(input, 100, stdin);

    input[strlen(input) - 1] = '\0';
    printf("%s: command not found\n", input);
  }
  return 0;
}
