#ifndef LIBTERMUX_EXEC__C__EXEC___H
#define LIBTERMUX_EXEC__C__EXEC___H

#include <stdbool.h>

#include <termux/termux_core__c/v1/shell/command/environment/UnixShellEnvironment.h>
#include <termux/termux_core__c/v1/termux/file/TermuxFile.h>
#include <termux/termux_core__c/v1/termux/shell/command/environment/TermuxShellEnvironment.h>

#ifdef __cplusplus
extern "C" {
#endif



/** The path to android system linker. */
#if UINTPTR_MAX == 0xffffffff
#define SYSTEM_LINKER_PATH "/system/bin/linker";
#elif UINTPTR_MAX == 0xffffffffffffffff
#define SYSTEM_LINKER_PATH "/system/bin/linker64";
#endif





/*
 * For the `execve()` system call, the kernel imposes a maximum length
 * limit on script shebang including the `#!` characters at the start
 * of a script. For Linux kernel `< 5.1`, the limit is `128`
 * characters and for Linux kernel `>= 5.1`, the limit is `256`
 * characters as per `BINPRM_BUF_SIZE` including the null `\0`
 * terminator.
 *
 * If `libtermux-exec-ld-preload.so` is set in `LD_PRELOAD` and
 * `TERMUX_EXEC__INTERCEPT_EXECVE` is enabled, then shebang limit is
 * increased to `340` characters defined by `FILE_HEADER__BUFFER_LEN`
 * as shebang is read and script is passed to interpreter as an
 * argument by `termux-exec` manually. Increasing limit to `340` also
 * fixes issues for older Android kernel versions where limit is
 * `128`. The limit is increased to `340`, because `BINPRM_BUF_SIZE`
 * would be set based on the assumption that rootfs is at `/`, so we
 * add Termux rootfs directory max length to it.
 *
 * - https://man7.org/linux/man-pages/man2/execve.2.html
 * - https://en.wikipedia.org/wiki/Shebang_(Unix)#Character_interpretation
 * - https://cs.android.com/android/kernel/superproject/+/0dc2b7de045e6dcfff9e0dfca9c0c8c8b10e1cf3:common/fs/binfmt_script.c;l=34
 * - https://cs.android.com/android/kernel/superproject/+/0dc2b7de045e6dcfff9e0dfca9c0c8c8b10e1cf3:common/include/linux/binfmts.h;l=64
 * - https://cs.android.com/android/kernel/superproject/+/0dc2b7de045e6dcfff9e0dfca9c0c8c8b10e1cf3:common/include/uapi/linux/binfmts.h;l=18
 *
 *
 * The running a script in `bash`, and the interpreter length is
 * `>= 128` (`BINPRM_BUF_SIZE`) and `execve()` system call returns
 * `ENOEXEC` (`Exec format error`), then `bash` will read the file
 * and run it as `bash` shell commands.
 * If interpreter length was `< 128` and `execve()` returned some
 * other error than `ENOEXEC`, then `bash` will try to give a
 * meaningful error.
 * - If script was not executable: `bash: <script_path>: Permission denied`
 * - If script was a directory: `bash: <script_path>: Is a directory`
 * - If `ENOENT` was returned since interpreter file was not found:
 *   `bash: <script_path>: cannot execute: required file not found`
 * - If some unhandled errno was returned, like interpreter file was a directory:
 *   `bash: <script_path>: <interpreter>: bad interpreter`
 *
 * - https://github.com/bminor/bash/blob/bash-5.2/execute_cmd.c#L5929
 * - https://github.com/bminor/bash/blob/bash-5.2/execute_cmd.c#L5964
 * - https://github.com/bminor/bash/blob/bash-5.2/execute_cmd.c#L5988
 * - https://github.com/bminor/bash/blob/bash-5.2/execute_cmd.c#L6048
 */

/**
 * The max length for entire shebang line for the Linux kernel `>= 5.1` defined by `BINPRM_BUF_SIZE`.
 * Add `FILE_HEADER__` scope to prevent conflicts by header importers.
 *
 * Default value: `256`
 */
#define FILE_HEADER__BINPRM_BUF_SIZE 256

/**
 * The max length for interpreter path in the shebang for `termux-exec`.
 *
 * Default value: `340`
 */
#define FILE_HEADER__INTERPRETER_PATH___MAX_LEN (TERMUX__ROOTFS_DIR___MAX_LEN + FILE_HEADER__BINPRM_BUF_SIZE - 1) // The `- 1` is to only allow one null `\0` terminator.

/**
 * The max length for interpreter arg in the shebang for `termux-exec`.
 *
 * This is same as `FILE_HEADER__BINPRM_BUF_SIZE`.  These is no way to
 * divide `BINPRM_BUF_SIZE` between path and arg, so we give it full
 * buffer size in case it needs it.
 *
 * Default value: `256`
 */
#define FILE_HEADER__INTERPRETER_ARG___MAX_LEN FILE_HEADER__BINPRM_BUF_SIZE

/**
 * The max length for entire shebang line for `termux-exec`.
 *
 * This is same as `FILE_HEADER__INTERPRETER_PATH___MAX_LEN`.
 *
 * Default value: `340`
 */
#define FILE_HEADER__BUFFER_LEN FILE_HEADER__INTERPRETER_PATH___MAX_LEN



/**
 * The info for the file header of an executable file.
 *
 * - https://refspecs.linuxfoundation.org/elf/gabi4+/ch4.eheader.html
 */
struct FileHeaderInfo {
    /** Whether executable is an ELF file instead of a script. */
    bool isElf;

    /**
     * Whether the `Elf32_Ehdr.e_machine != EM_NATIVE`, i.e executable
     * file is 32-bit binary on a 64-bit host.
     */
    bool isNonNativeElf;

    /**
     * The original interpreter path set in the executable file that is
     * not normalized, absolutized or prefixed.
     */
    char const *origInterpreterPath;

    /**
     * The interpreter path set in the executable file that is
     * normalized, absolutized and prefixed.
     */
    char const *interpreterPath;
    /** The underlying buffer for `interpreterPath`. */
    char interpreterPathBuffer[FILE_HEADER__INTERPRETER_PATH___MAX_LEN];

    /** The arguments to the interpreter set in the executable file. */
    char const *interpreterArg;
    /** The underlying buffer for `interpreterArg`. */
    char interpreterArgBuffer[FILE_HEADER__INTERPRETER_ARG___MAX_LEN];
};



/** The host native architecture. */
#ifdef __aarch64__
#define EM_NATIVE EM_AARCH64
#elif defined(__arm__) || defined(__thumb__)
#define EM_NATIVE EM_ARM
#elif defined(__x86_64__)
#define EM_NATIVE EM_X86_64
#elif defined(__i386__)
#define EM_NATIVE EM_386
#else
#error "unknown arch"
#endif


/**
 * The list of variables that are unset by `modifyExecEnv()` if
 * `unsetLdVarsFromEnv` is `true`.
 */
static const char *LD_VARS_TO_UNSET[] __attribute__ ((unused)) = { ENV_PREFIX__LD_LIBRARY_PATH, ENV_PREFIX__LD_PRELOAD };
static int LD_VARS_TO_UNSET_SIZE __attribute__ ((unused)) = 2;


/**
 * Intercept for the `execve()` method in `unistd.h`.
 *
 * If `isTermuxExecExecveInterceptEnabled()` returns `true`,
 * then `execveIntercept()` will be called, otherwise
 * `execveSyscall()`.
 *
 * - https://man7.org/linux/man-pages/man3/exec.3.html
 */
int execveIntercept(bool intercept, const char *executablePath, char *const argv[], char *const envp[]);



/**
 * Inspect file header and set `FileHeaderInfo`.
 *
 * @param termuxPrefixDir The **normalized** path to termux prefix
 *                        directory. If `NULL`, then path returned by
 *                        `getTermuxPrefixDirFromEnvOrDefault()`
 *                        will be used by calling `getTermuxPrefixDir()`.
 * @param header The file header read from the executable file.
 *               The `FILE_HEADER__BUFFER_LEN` should be used as buffer size
 *               when reading.
 * @param headerLength The actual length of the header that was read.
 * @param info The `FileHeaderInfo` to set.
 */
int inspectFileHeader(const char *termuxPrefixDir, char *header, size_t headerLength,
    struct FileHeaderInfo *info);



/**
 * Whether variables in `LD_VARS_TO_UNSET` should be unset before `exec()`
 * to prevent issues when executing system binaries that are caused
 * if they are set.
 *
 * @param isNonNativeElf The value for `FileHeaderInfo.isNonNativeElf`
 *        for the executable file.
 * @param executablePath The **normalized** executable path to check.
 * @return Returns `true` if `isNonNativeElf` equals `true` or
 * `executablePath` starts with `/system/`, but does not equal
 * `/system/bin/sh`, `system/bin/linker` or `/system/bin/linker64`.
 *
 */
bool shouldUnsetLDVarsFromEnv(bool isNonNativeElf, const char *executablePath);

/**
 * Modify the environment for `execve()`.
 *
 * @param envp The current environment pointer.
 * @param newEnvpPointer The new environment pointer to set.
 * @param envTermuxProcSelfExe If set, then it will overwrite or
 *                                 set the `ENV_PREFIX__TERMUX_EXEC__PROC_SELF_EXE`
 *                                 env variable.
 * @param unsetLdVarsFromEnv If `true`, then variables in
 *                               `LD_VARS_TO_UNSET` will be unset.
 * @return Returns `0` if successfully modified the env, otherwise
 * `-1` on failures. Its the callers responsibility to call `free()`
 * on the `newEnvpPointer` passed.
 */
int modifyExecEnv(char *const *envp, char ***newEnvpPointer,
    char** envTermuxProcSelfExe, bool unsetLdVarsFromEnv);



/**
 * Modify the arguments for `execve()`.
 *
 * If `interpreterSet` is set, then `argv[0]` will be set to
 * `FileHeaderInfo.origInterpreterPath`, otherwise the original
 * `argv[0]` passed to `execve()` will be preserved.
 *
 * If `interpreterSet` is set, then `FileHeaderInfo.interpreterArg`
 * will be appended if set, followed by the `origExecutablePath`
 * passed to `execve()`.
 *
 * Any additional arguments to `execve()` will be appended after this.
 *
 * @param argv The current arguments pointer.
 * @param newArgvPointer The new arguments pointer to set.
 * @param origExecutablePath The originnal executable path passed to
 *                             `execve()`.
 * @param executablePath The **normalized** executable or interpreter
 *                        path that will actually be executed.
 * @param interpreterSet Whether a interpreter is set in the executable
 *                        file.
 * @param info The `FileHeaderInfo` for the executable file.
 * @return Returns `0` if successfully modified the args, otherwise
 * `-1` on failures. Its the callers responsibility to call `free()`
 * on the `newArgvPointer` passed.
 */
int modifyExecArgs(char *const *argv, const char ***newArgvPointer,
    const char *origExecutablePath, const char *executablePath,
    bool interpreterSet, struct FileHeaderInfo *info);



#ifdef __cplusplus
}
#endif

#endif // LIBTERMUX_EXEC__C__EXEC___H
