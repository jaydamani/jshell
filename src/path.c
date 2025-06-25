#include <stdlib.h>
#include <linux/limits.h>
#include <string.h>
#define PATH_SEP "/"

int sep_len = strlen(PATH_SEP);

int append_path(const char *dir,const char *file, char* res) {
  char* buf = malloc(PATH_MAX);
  strcpy(buf, dir);

  if (strcmp(&buf[strlen(buf) - sep_len], PATH_SEP) != 0) {
    strcat(buf, PATH_SEP);
  }

  strcat(buf, file);
  strcpy(res, buf);
  free(buf);
  return 0;
}

// int main(void) {
//     char *a = append_path("a", "b");
//     printf("appended\n");
//     printf("%s\n", a);
// }
