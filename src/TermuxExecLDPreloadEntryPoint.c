#define _GNU_SOURCE
#include <stdarg.h>
#include <stdbool.h>
#include <unistd.h>

#include <termux/termux_exec__c/v1/termux/api/termux_exec/TermuxExecInit.h>
#include <termux/termux_exec__c/v1/termux/api/termux_exec/exec/ExecIntercept.h>
#include <termux/termux_exec__c/v1/termux/api/termux_exec/exec/ExecVariantsIntercept.h>

/**
 * This file defines functions intercepted by `libtermux-exec-ld-preload.so` using `$LD_PRELOAD`.
 *
 * All exported functions must explicitly enable `default` visibility
 * with `__attribute__((visibility("default")))` as `libtermux-exec-ld-preload.so`
 * is compiled with `-fvisibility=hidden` so that no other internal
 * functions are exported.
 *
 * You can check exported symbols for dynamic linking after building with:
 * `nm --demangle --dynamic --defined-only --extern-only /home/builder/.termux-build/termux-exec/src/build/output/usr/lib/libtermux-exec-ld-preload.so`.
 */



__attribute__((visibility("default")))
int execl(const char *name, const char *arg, ...) {
    initProcess(true);

    va_list ap;
    va_start(ap, arg);
    int result = execlIntercept(true, ExecL, name, arg, ap);
    va_end(ap);
    return result;
}

__attribute__((visibility("default")))
int execlp(const char *name, const char *arg, ...) {
    initProcess(true);

    va_list ap;
    va_start(ap, arg);
    int result = execlIntercept(true, ExecLP, name, arg, ap);
    va_end(ap);
    return result;
}

__attribute__((visibility("default")))
int execle(const char *name, const char *arg, ...) {
    initProcess(true);

    va_list ap;
    va_start(ap, arg);
    int result = execlIntercept(true, ExecLE, name, arg, ap);
    va_end(ap);
    return result;
}

__attribute__((visibility("default")))
int execv(const char *name, char *const *argv) {
    initProcess(true);

    return execvIntercept(true, name, argv);
}

__attribute__((visibility("default")))
int execvp(const char *name, char *const *argv) {
    initProcess(true);

    return execvpIntercept(true, name, argv);
}

__attribute__((visibility("default")))
int execvpe(const char *name, char *const *argv, char *const *envp) {
    initProcess(true);

    return execvpeIntercept(true, name, argv, envp);
}

__attribute__((visibility("default")))
int fexecve(int fd, char *const *argv, char *const *envp) {
    initProcess(true);

    return fexecveIntercept(true, fd, argv, envp);
}

__attribute__((visibility("default")))
int execve(const char *name, char *const argv[], char *const envp[]) {
    initProcess(true);

    return execveIntercept(true, name, argv, envp);
}
