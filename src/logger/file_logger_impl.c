#define _GNU_SOURCE
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <linux/limits.h>

#include "logger.h"
#include "file_logger_impl.h"
#include "../os/safe_strerror.h"

const struct ILogger sFileLoggerImpl = {
    .printMessage = printMessageToFile,
};


static FILE *sLogFileStream = NULL;

static char sLogFilePathBuffer[PATH_MAX];

/** The log file path. */
static const char* sLogFilePath = NULL;

static bool sWarnNoLogFileSet = true;


void printMessageToFile(bool logOnStderr, const char *message) {
    (void)logOnStderr;

    if (sLogFileStream == NULL) {
        if (sWarnNoLogFileSet) {
            fprintf(stderr, "No log file set");
            sWarnNoLogFileSet = false;
        }
        return;
    }

    fprintf(sLogFileStream, "%s", message);
    fflush(sLogFileStream);
}



FILE* getLogFileStream() {
    return sLogFileStream;
}

const char* getLogFilePath() {
    return sLogFilePath;
}

int setLogFilePath(const char* logFilePath) {
    sWarnNoLogFileSet = true;
    closeLogFile();

    if (logFilePath == NULL) {
        return 0;
    }

    size_t logFilePathLength = strlen(logFilePath);
    if (logFilePathLength < 1) {
        return 0;
    }

    if (sizeof(sLogFilePathBuffer) <= logFilePathLength) {
        fprintf(stderr, "The log file path '%s' with length '%zu' is too long to fit in the buffer with length '%zu'",
            logFilePath, logFilePathLength, sizeof(sLogFilePathBuffer));
        closeLogFile();
        return -1;
    }

    strcpy(sLogFilePathBuffer, logFilePath);
    sLogFilePath = sLogFilePathBuffer;

    sLogFileStream = fopen(sLogFilePath, "w");
    if (sLogFileStream == NULL) {
        char strerrorBuffer[STRERROR_BUFFER_SIZE];
        safe_strerror_r(errno, strerrorBuffer, sizeof(strerrorBuffer));

        fprintf(stderr, "Failed to open log file '%s': %s\n", sLogFilePath, strerrorBuffer);
        closeLogFile();
        return -1;
    }

    sWarnNoLogFileSet = false;

    return 0;
}

int closeLogFile() {
    if (sLogFileStream != NULL) {
        fclose(sLogFileStream);
        sLogFileStream = NULL;
    }
    sLogFilePath = NULL;
    sLogFilePathBuffer[0] = '\0';
    return 0;
}
