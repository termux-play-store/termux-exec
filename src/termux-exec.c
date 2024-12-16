#define _GNU_SOURCE
#include <stdarg.h>
#include <stdbool.h>
#include <unistd.h>

#include "termux-exec-init.h"
#include "exec/exec.h"
#include "exec/exec_variants.h"
#include "logger/logger.h"
#include "termux/termux_env.h"

/**
 * This file defines functions intercepted by `libtermux-exec.so` using `LD_PRELOAD`.
 *
 * All exported functions must explicitly enable `default` visibility
 * with `__attribute__((visibility("default")))` as `libtermux-exec.so`
 * is compiled with `-fvisibility=hidden` so that no other internal
 * functions get exported.
 *
 * You can check exported symbols for dynamic linking after building with:
 * `nm --demangle --dynamic --defined-only --extern-only /home/builder/.termux-build/termux-exec/src/build/usr/lib/libtermux-exec.so`.
 */



__attribute__((visibility("default")))
int execl(const char *name, const char *arg, ...) {
    initProcess(true);

    va_list ap;
    va_start(ap, arg);
    int result = execl_hook(true, ExecL, name, arg, ap);
    va_end(ap);
    return result;
}

__attribute__((visibility("default")))
int execlp(const char *name, const char *arg, ...) {
    initProcess(true);

    va_list ap;
    va_start(ap, arg);
    int result = execl_hook(true, ExecLP, name, arg, ap);
    va_end(ap);
    return result;
}

__attribute__((visibility("default")))
int execle(const char *name, const char *arg, ...) {
    initProcess(true);

    va_list ap;
    va_start(ap, arg);
    int result = execl_hook(true, ExecLE, name, arg, ap);
    va_end(ap);
    return result;
}

__attribute__((visibility("default")))
int execv(const char *name, char *const *argv) {
    initProcess(true);

    return execv_hook(true, name, argv);
}

__attribute__((visibility("default")))
int execvp(const char *name, char *const *argv) {
    initProcess(true);

    return execvp_hook(true, name, argv);
}

__attribute__((visibility("default")))
int execvpe(const char *name, char *const *argv, char *const *envp) {
    initProcess(true);

    return execvpe_hook(true, name, argv, envp);
}

__attribute__((visibility("default")))
int fexecve(int fd, char *const *argv, char *const *envp) {
    initProcess(true);

    return fexecve_hook(true, fd, argv, envp);
}

__attribute__((visibility("default")))
int execve(const char *name, char *const argv[], char *const envp[]) {
    initProcess(true);

    return execve_hook(true, name, argv, envp);
}
