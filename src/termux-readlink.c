#define _GNU_SOURCE
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <sys/syscall.h>
#include <unistd.h>

__attribute__((visibility("default"))) ssize_t readlink(const char *restrict pathname, char *restrict buf,
                                                        size_t bufsiz) {
  if (strcmp(pathname, "/proc/self/exe") == 0) {
    const char *termux_self_exe = getenv("TERMUX_EXEC__PROC_SELF_EXE");
    if (termux_self_exe) {
      size_t termux_self_exe_len = strlen(termux_self_exe);
      size_t bytes_to_copy = (termux_self_exe_len < bufsiz) ? termux_self_exe_len : bufsiz;
      memcpy(buf, termux_self_exe, bytes_to_copy);
      return bytes_to_copy;
    }
  }

  return syscall(SYS_readlinkat, AT_FDCWD, pathname, buf, bufsiz);
}
