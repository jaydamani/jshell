#include <stdlib.h>
#include <linux/limits.h>
#include <string.h>
#include <unistd.h>
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

char *find_path_cmd(char *cmd) {
  char *path;
  char *path_var = strdup(getenv("PATH"));
  char *file_path = malloc(PATH_MAX);
  char *res = NULL;

  while ((path = strtok_r(path_var, ":", &path_var))) {
    append_path(path, cmd, file_path);
    realpath(file_path, file_path);
    if (access(file_path, X_OK) == 0) {
      res = file_path;
      break;
    }
  }
  if (res == NULL) {
    free(file_path);
  }
  return res;
}


// int main(void) {
//     char *a = append_path("a", "b");
//     printf("appended\n");
//     printf("%s\n", a);
// }
