#define _GNU_SOURCE
#include <assert.h>
#include <elf.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/stat.h>
#include <sys/types.h>

#include <termux/termux_core__c/v1/TermuxCoreLibraryConfig.h>
#include <termux/termux_core__c/v1/data/AssertUtils.h>
#include <termux/termux_core__c/v1/data/DataUtils.h>
#include <termux/termux_core__c/v1/file/FileUtils.h>
#include <termux/termux_core__c/v1/logger/Logger.h>
#include <termux/termux_core__c/v1/termux/file/TermuxFile.h>
#include <termux/termux_core__c/v1/shell/command/environment/UnixShellEnvironment.h>
#include <termux/termux_core__c/v1/termux/shell/command/environment/TermuxShellEnvironment.h>

#include <termux/termux_exec__c/v1/TermuxExecLibraryConfig.h>
#include <termux/termux_exec__c/v1/termux/shell/command/environment/termux_exec/TermuxExecShellEnvironment.h>



static const char* LOG_TAG = "ub-tests";


static void init();
static void initLogger();
static void runTests();



#include "termux/api/termux_exec/exec/ExecIntercept_UnitBinaryTests.c"



__attribute__((visibility("default")))
int main() {
    init();

    logVVerbose(LOG_TAG, "main()");

    runTests();

    return 0;
}



static void init() {
    errno = 0;

    libtermux_core__c__setIsRunningTests(true);
    libtermux_exec__c__setIsRunningTests(true);

    initLogger();
}

static void initLogger() {
    setDefaultLogTagAndPrefix("lib" TERMUX__LNAME "-exec_c");
    setCurrentLogLevel(getTermuxExecTestsLogLevel());
    setLogFormatMode(LOG_FORMAT_MODE__TAG_AND_MESSAGE);
}



void runTests() {

    logDebug(LOG_TAG, "runTests(start)");

    ExecIntercept_runTests();

    logDebug(LOG_TAG, "runTests(end)");

}
