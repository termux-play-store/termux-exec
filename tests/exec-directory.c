#include <errno.h>
#include <stdio.h>
#include <unistd.h>

int main(int, char **argv, char **env) {
  execve(".", argv, env);
  printf("errno = %d\n", errno);
  execve("/", argv, env);
  printf("errno = %d\n", errno);
  return 0;
}
