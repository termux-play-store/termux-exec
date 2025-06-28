#include <errno.h>
#include <stdio.h>
#include <unistd.h>

void exec_debug(char *path) {
  int ret = execl(path, "--arg", (char *)NULL);
  if (ret != -1) {
    fprintf(stderr, "Unexpected return value when execl():ing non-existing file: %d\n", ret);
  } else {
    printf("errno = %d\n", errno);
  }
}

int main(int argc, char **argv) {
  if (argc != 2) {
    fprintf(stderr, "Unexpected argc=%d\n", argc);
    return 1;
  }
  exec_debug("/this-file-does-not-exist");
  exec_debug(argv[1]);
  return 0;
}
