#include <stdbool.h>

#include <termux/termux_core__c/v1/logger/Logger.h>

#include <termux/termux_exec__c/v1/termux/api/termux_exec/TermuxExecInit.h>
#include <termux/termux_exec__c/v1/termux/shell/command/environment/termux_exec/TermuxExecShellEnvironment.h>


static bool sInitLogger = true;
static bool sIgnoreExit = false;



int initProcess(bool shouldLogVersion) {
    // Sometimes when a process starts, errno is set to values like
    // EINVAL (22) and (ECHILD 10). Noticed on Android 11 (aarch64) and
    // Android 14 (x86_64).
    // It is not being caused by `termux-exec` as it happens even
    // without `LD_PRELOAD` being set.
    // Moreover, errno is 0 before `execveSyscall()` is called by
    // `execveIntercept()` to replace the process, but in the `main()`
    // of new process, errno is not zero, so something happens during
    // the `syscall()` itself or in callers of `main()`. And manually
    // setting errno before `execveSyscall()` does not transfer it
    // to `main()` of new process.
    // Programs should set errno to `0` at init themselves.
    // We unset it here since programs should have already handled their
    // errno if it was set by something else and `termux-exec` library
    // also has checks to error out if errno is set in various places,
    // like initially in `stringToInt()` called by `getTermuxExecLogLevel()`.
    // Saving errno is useless as it will not be transferred anyways.
    // - https://wiki.sei.cmu.edu/confluence/display/c/ERR30-C.+Take+care+when+reading+errno
    errno = 0;

    return initLogger(shouldLogVersion);
}

int initLogger(bool shouldLogVersion) {
    if (sInitLogger) {
        setDefaultLogTagAndPrefix(TERMUX__LNAME);
        setCurrentLogLevel(getTermuxExecLogLevel());
        setCacheLogPid(true);
        setLogFormatMode(LOG_FORMAT_MODE__PID_PRIORITY_TAG_AND_MESSAGE);
        sInitLogger = false;

        if (shouldLogVersion) {
            logErrorVVerbose("", "TERMUX_EXEC_PKG__VERSION: '%s'", TERMUX_EXEC_PKG__VERSION);
        }
    }
    return 0;
}



void setIgnoreExit(bool state) {
    sIgnoreExit = state;
}

int exitProcess() {
    if (!sIgnoreExit) {
        sInitLogger = true;
    }
    return 0;
}
