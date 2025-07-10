#include <errno.h>
#include <stdio.h>
#include <unistd.h>

int main(int, char **argv, char** environ) {
  execve(".", argv, environ);
  printf("errno = %d\n", errno);
  return 0;
}
