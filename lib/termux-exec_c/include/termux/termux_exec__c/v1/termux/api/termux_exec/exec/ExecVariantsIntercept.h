#ifndef LIBTERMUX_EXEC__C__EXEC_VARIANTS___H
#define LIBTERMUX_EXEC__C__EXEC_VARIANTS___H

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif



/**
 * Intercepts for all the `exec()` family of functions in `unistd.h`, other
 * than `execve()`, whose intercept `execveIntercept()` is declared
 * in `ExecIntercept.h`.
 *
 * - https://man7.org/linux/man-pages/man3/exec.3.html
 *
 * These `exec()` variants end up calling `execve()` and are ported
 * from `libc/bionic/exec.cpp`.
 *
 * For Android `< 14` intercepting `execve()` was enough.
 * For Android `>= 14` requires intercepting the entire `exec()` family
 * of functions. It might be related to the `3031a7e4` commit in `bionic`,
 * in which `exec.cpp` added `memtag-stack` for `execve()` and shifted
 * to calling `__execve()` internally in it.
 *
 * Intercepting the entire family should also solve some issues on older
 * Android versions, check `libc/bionic/exec.cpp` git history.
 *
 * Tests for each `exec` family is done by `run_all_exec_wrappers_test`
 * in `TermuxExecRuntimeBinaryTests.c`.
 *
 * - https://cs.android.com/android/platform/superproject/+/android-14.0.0_r1:bionic/libc/bionic/exec.cpp
 * - https://cs.android.com/android/platform/superproject/+/android-14.0.0_r1:bionic/libc/SYSCALLS.TXT;l=68
 * - https://cs.android.com/android/_/android/platform/bionic/+/3031a7e45eb992d466610bec3bb589c41b74992b
 */


enum EXEC_VARIANTS { ExecVE, ExecL, ExecLP, ExecLE, ExecV, ExecVP, ExecVPE, FExecVE };

static const char * const EXEC_VARIANTS_STR[] = {
    [ExecVE]  = "execve",
    [ExecL]   = "execl", [ExecLP] = "execlp", [ExecLE]  = "execle",
    [ExecV]   = "execv", [ExecVP] = "execvp", [ExecVPE] = "execvpe",
    [FExecVE] = "fexecve"
};


/**
 * Intercept for the `execl()`, `execle()` and `execlp()` functions in `unistd.h`
 * which redirects the call to `execve()`.
 */
int execlIntercept(bool wasIntercepted, int variant, const char *name, const char *argv0, va_list ap);

/**
 * Intercept for the `execv()` function in `unistd.h` which redirects the call to `execve()`.
 */
int execvIntercept(bool wasIntercepted, const char *name, char *const *argv);

/**
 * Intercept for the `execvp()` function in `unistd.h` which redirects the call to `execve()`.
 */
int execvpIntercept(bool wasIntercepted, const char *name, char *const *argv);

/**
 * Intercept for the `execvpe()` function in `unistd.h` which redirects the call to `execve()`.
 */
int execvpeIntercept(bool wasIntercepted, const char *name, char *const *argv, char *const *envp);

/**
 * Intercept for the `fexecve()` function in `unistd.h` which redirects the call to `execve()`.
 */
int fexecveIntercept(bool wasIntercepted, int fd, char *const *argv, char *const *envp);



#ifdef __cplusplus
}
#endif

#endif // LIBTERMUX_EXEC__C__EXEC_VARIANTS___H
