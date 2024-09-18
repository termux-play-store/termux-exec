#define _DEFAULT_SOURCE
#include <linux/limits.h>
#include <stdio.h>
#include <unistd.h>

int main() {
  char buf[PATH_MAX + 1];

  ssize_t res = readlink("/proc/self/exe", buf, PATH_MAX);
  if (res < 0) {
    perror("readlink()");
    return 1;
  }
  buf[res] = 0;
  printf("%s\n", buf);
  return 0;
}

