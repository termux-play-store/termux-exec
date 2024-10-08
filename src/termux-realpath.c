#define _GNU_SOURCE
#include <dlfcn.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <sys/syscall.h>
#include <unistd.h>

__attribute__((visibility("default"))) char* _Nullable realpath(const char* _Nonnull path, char* _Nullable resolved) {
      char* (*orig_realpath)(const char*, char*);
      orig_realpath = dlsym(RTLD_NEXT, "realpath");

  if (strcmp(path, "/proc/self/exe") == 0) {
    const char *termux_self_exe = getenv("TERMUX_EXEC__PROC_SELF_EXE");
    if (termux_self_exe) {
		   path = termux_self_exe;
    }
  }

  return orig_realpath(path, resolved);
}
