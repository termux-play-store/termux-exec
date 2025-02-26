#define _GNU_SOURCE
#include <errno.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include <termux/termux_core__c/v1/logger/Logger.h>
#include <termux/termux_core__c/v1/shell/command/environment/UnixShellEnvironment.h>
#include <termux/termux_core__c/v1/termux/shell/command/environment/TermuxShellEnvironment.h>

#include <termux/termux_exec__c/v1/termux/shell/command/environment/termux_exec/TermuxExecShellEnvironment.h>



int getTermuxExecLogLevel() {
    return getLogLevelFromEnv(ENV__TERMUX_EXEC__LOG_LEVEL);
}



bool isTermuxExecExecveInterceptEnabled() {
    return getBoolEnvValue(ENV__TERMUX_EXEC__INTERCEPT_EXECVE, E_DEF_VAL__TERMUX_EXEC__INTERCEPT_EXECVE);
}

int getTermuxExecSystemLinkerExecConfig() {
    int def = E_DEF_VAL__TERMUX_EXEC__SYSTEM_LINKER_EXEC;
    const char* value = getenv(ENV__TERMUX_EXEC__SYSTEM_LINKER_EXEC);
    if (value == NULL || strlen(value) < 1) {
        return def;
    } else if (strcmp(value, "disable") == 0) {
        return 0;
    } else if (strcmp(value, "enable") == 0) {
        return 1;
    } else if (strcmp(value, "force") == 0) {
        return 2;
    }
    return def;
}



int getTermuxExecTestsLogLevel() {
    return getLogLevelFromEnv(ENV__TERMUX_EXEC__TESTS__LOG_LEVEL);
}
