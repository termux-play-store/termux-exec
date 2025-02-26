#define _GNU_SOURCE
#include <stddef.h>
#include <errno.h>
#include <fcntl.h>
#include <regex.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/wait.h>

#include <linux/limits.h>

#ifdef __ANDROID__
#include <android/api-level.h>
#endif

#include <termux/termux_core__c/v1/TermuxCoreLibraryConfig.h>
#include <termux/termux_core__c/v1/data/AssertUtils.h>
#include <termux/termux_core__c/v1/data/DataUtils.h>
#include <termux/termux_core__c/v1/logger/Logger.h>
#include <termux/termux_core__c/v1/os/process/ForkUtils.h>
#include <termux/termux_core__c/v1/shell/command/environment/UnixShellEnvironment.h>
#include <termux/termux_core__c/v1/termux/shell/command/environment/TermuxShellEnvironment.h>
#include <termux/termux_exec__c/v1/termux/shell/command/environment/termux_exec/TermuxExecShellEnvironment.h>

#include <termux/termux_exec__c/v1/TermuxExecLibraryConfig.h>



static const char* LOG_TAG = "rb-tests";

static uid_t UID;

#define TERMUX_EXEC__TESTS__TESTS_PATH TERMUX__PREFIX "/libexec/installed-tests/termux-exec"

extern char **environ;


static void init();
static void initLogger();
static void initChild(ForkInfo *info);
static void runTests();



#include "termux/api/termux_exec/exec/ExecIntercept_RuntimeBinaryTests.c"



__attribute__((visibility("default")))
int main() {
    init();

    logVVerbose(LOG_TAG, "main()");

    runTests();

    return 0;
}



static void init() {
    errno = 0;

    UID = geteuid();

    libtermux_core__c__setIsRunningTests(true);
    libtermux_exec__c__setIsRunningTests(true);

    initLogger();
}

static void initLogger() {
    setDefaultLogTagAndPrefix("lib" TERMUX__LNAME "-exec_c");
    setCurrentLogLevel(getTermuxExecTestsLogLevel());
    setLogFormatMode(LOG_FORMAT_MODE__TAG_AND_MESSAGE);
}

static void initChild(ForkInfo *info) {
    (void)info;
    initLogger();
}



void runTests() {

    logDebug(LOG_TAG, "runTests(start)");

    ExecIntercept_runTests();

    logDebug(LOG_TAG, "runTests(end)");

}
