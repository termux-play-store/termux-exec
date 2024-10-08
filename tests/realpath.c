#define _POSIX_C_SOURCE 1
#define _XOPEN_SOURCE 500
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv) {
  if (argc != 2) {
    fprintf(stderr, "Wrong arguments\n");
  }
  char buf[PATH_MAX];
  char *res = realpath(argv[1], buf);
  if (res) {
    printf("%s\n", buf);
  } else {
    perror("realpath()");
    exit(EXIT_FAILURE);
  }
  return 0;
}
