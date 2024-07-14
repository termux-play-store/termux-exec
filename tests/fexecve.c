#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

int main() {
  const char *path_to_echo = TERMUX_BASE_DIR "/usr/bin/sh";
  int fd = open(path_to_echo, O_RDONLY);
  if (fd < 0) {
    perror("open");
    return 1;
  }
  char *args[] = {"sh", "-c", "echo hello fexecve", NULL};
  char *env[] = {NULL};
  fexecve(fd, args, env);
  perror("fexecve");
  return 0;
}
