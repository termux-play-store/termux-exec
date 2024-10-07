#define _DEFAULT_SOURCE
#include <linux/limits.h>
#include <stdio.h>
#include <string.h>
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

  // Validate that reading /proc/$PID/exe gives the same result as /proc/self/exe
  char *proc_pid_exe_path;
  asprintf(&proc_pid_exe_path, "/proc/%ld/exe", (long)getpid());
  char pid_buf[PATH_MAX + 1];
  int fd = open(proc_pid_exe_path, O_RDONLY);
  if (fd <= 0) {
    perror("open()");
    return 1;
  }
  ssize_t pid_res = readlink(proc_pid_exe_path, pid_buf, PATH_MAX);
  if (pid_res <= 0) {
    perror("readlink()");
    return 1;
  }
  pid_buf[pid_res] = 0;
  if (strcmp(buf, pid_buf) != 0) {
    fprintf(stderr, "Mismatch - readlink(/proc/self/exe)='%s', readlink(/proc/$PID/exe)='%s'\n", buf, pid_buf);
    return 1;
  }

  return 0;
}
