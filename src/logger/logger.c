#define _GNU_SOURCE
#include <stdarg.h>
#include <stdbool.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

#include <sys/types.h>

#include "../os/safe_strerror.h"
#include "logger.h"
#include "standard_logger_impl.h"

static char sDefaultLogTagBuffer[LOGGER_TAG_MAX_LENGTH] = "Logger";

/** The default log tag used if log methods that do not require a `tag` are called. */
static const char* sDefaultLogTag = sDefaultLogTagBuffer;

static char sLogTagPrefixBuffer[LOGGER_TAG_MAX_LENGTH] = "";

/** The log tag prefixed before the `tag` passed to log methods that require a `tag` by `getFullLogTag()`. */
static const char* sLogTagPrefix = sLogTagPrefixBuffer;

/** The current log level. */
static int sCurrentLogLevel = DEFAULT_LOG_LEVEL;


/** The `ILogger` implementation to use for logging. */
static const struct ILogger* sLoggerImpl = &sStandardLoggerImpl;


/**
 * The log format mode for log entries as per `LOG_FORMAT_MODE__*`.
 */
static int sLogFormatMode = LOG_FORMAT_MODE__PID_PRIORITY_TAG_AND_MESSAGE;


/**
 * The pid of current process.
 *
 * - https://man7.org/linux/man-pages/man2/getpid.2.html
 * - https://cs.android.com/android/platform/superproject/+/android-13.0.0_r54:bionic/libc/bionic/pthread_internal.h
 * - https://cs.android.com/android/platform/superproject/+/android-13.0.0_r54:bionic/libc/bionic/getpid.cpp
 */
static pid_t sLogPid = -1;

/**
 * Whether to cache `sLogPid` that is used for
 * `LOG_FORMAT_MODE__PID_PRIORITY_TAG_AND_MESSAGE`.
 *
 * If enabled, and a process forks a new process, the cached pid value
 * of the parent will be cloned to the child and log entries will
 * contain wrong value in the child. So if enabling caching, then
 * call `updateLogPid()` on each fork manually to update the value.
 *
 * By default caching is disabled.
 */
static bool sCacheLogPid = false;



static const char* getFullLogTag(const char* tag, char *buffer, size_t buffer_len);





int getCurrentLogLevel() {
    return sCurrentLogLevel;
}

int setCurrentLogLevel(int currentLogLevel) {
    if (!isLogLevelValid(currentLogLevel)) {
        return sCurrentLogLevel;
    }

    sCurrentLogLevel = getLogLevelIfValidOtherwiseDefault(currentLogLevel);
    return sCurrentLogLevel;
}


bool isLogLevelValid(int logLevel) {
    return (logLevel >= MIN_LOG_LEVEL && logLevel <= MAX_LOG_LEVEL);
}

int getLogLevelIfValidOtherwiseDefault(int logLevel) {
    return isLogLevelValid(logLevel) ? logLevel : DEFAULT_LOG_LEVEL;
}



void setDefaultLogTagAndPrefix(const char* defaultLogTag) {
    if (defaultLogTag == NULL || strlen(defaultLogTag) < 1) return;
    setDefaultLogTag(defaultLogTag);

    char logTagPrefix[strlen(defaultLogTag) + 2];
    snprintf(logTagPrefix, sizeof(logTagPrefix), "%s.", defaultLogTag);
    setLogTagPrefix(logTagPrefix);
}


const char* getDefaultLogTag() {
    return sDefaultLogTag;
}

void setDefaultLogTag(const char* defaultLogTag) {
    if (defaultLogTag == NULL) return;

    size_t defaultLogTagLength = strlen(defaultLogTag);
    if (defaultLogTagLength < 1 || sizeof(sDefaultLogTagBuffer) <= defaultLogTagLength) return;

    strcpy(sDefaultLogTagBuffer, defaultLogTag);
}


const char* getLogTagPrefix() {
    return sLogTagPrefix;
}

void setLogTagPrefix(const char* logTagPrefix) {
    if (logTagPrefix == NULL) return;

    size_t logTagPrefixLength = strlen(logTagPrefix);
    if (sizeof(sLogTagPrefixBuffer) <= logTagPrefixLength) return;

    strcpy(sLogTagPrefixBuffer, logTagPrefix);
}


const struct ILogger* getLoggerImpl() {
    return sLoggerImpl;
}

void setLoggerImpl(const struct ILogger* loggerImpl) {
    if (loggerImpl != NULL)
        sLoggerImpl = loggerImpl;
}


int getLogFormatMode() {
    return sLogFormatMode;
}

void setLogFormatMode(int logFormatMode) {
    if (logFormatMode == LOG_FORMAT_MODE__MESSAGE || \
            logFormatMode == LOG_FORMAT_MODE__TAG_AND_MESSAGE || \
            logFormatMode == LOG_FORMAT_MODE__PID_PRIORITY_TAG_AND_MESSAGE) {
        sLogFormatMode = logFormatMode;
    }
}


bool getCacheLogPid() {
    return sCacheLogPid;
}

void setCacheLogPid(bool cacheLogPid) {
    sCacheLogPid = cacheLogPid;
}



pid_t getLogPid() {
    if (sCacheLogPid) {
        if (sLogPid < 0)
            sLogPid = getpid();
        return sLogPid;
    } else {
        return getpid();
    }
}

void updateLogPid() {
    sLogPid = getpid();
}





static void printMessage(char logPriorityChar, bool logOnStderr,
    const char* tag, const char* fmt, va_list args) {
    int errnoCode = errno;

    int logFormatMode = getLogFormatMode();

    char finalMessage[LOGGER_ENTRY_MAX_PAYLOAD];

    if (logFormatMode == LOG_FORMAT_MODE__MESSAGE) {
        char currentMessage[LOGGER_ENTRY_MAX_PAYLOAD - 1];
        vsnprintf(currentMessage, sizeof(currentMessage), fmt, args);

        snprintf(finalMessage, sizeof(finalMessage), "%s\n", currentMessage);
    } else if (logFormatMode == LOG_FORMAT_MODE__TAG_AND_MESSAGE) {
        const char* finalTag = tag != NULL ? tag : "null";

        char currentMessage[LOGGER_ENTRY_MAX_PAYLOAD - LOGGER_TAG_MAX_LENGTH - 2 - 1];
        vsnprintf(currentMessage, sizeof(currentMessage), fmt, args);

        snprintf(finalMessage, sizeof(finalMessage), "%-8s: %s\n",
            finalTag, currentMessage);
    } else if (logFormatMode == LOG_FORMAT_MODE__PID_PRIORITY_TAG_AND_MESSAGE) {
        const char* finalTag = tag != NULL ? tag : "null";

        char currentMessage[LOGGER_ENTRY_MAX_PAYLOAD - 5 - 1 - 1 -1 - LOGGER_TAG_MAX_LENGTH - 2 - 1];
        vsnprintf(currentMessage, sizeof(currentMessage), fmt, args);

        snprintf(finalMessage, sizeof(finalMessage), "%5d %c %-8s: %s\n",
            getLogPid(), logPriorityChar, finalTag, currentMessage);
    }

    sLoggerImpl->printMessage(logOnStderr, finalMessage);

    errno = errnoCode;
}

#define ILOGGER_IMPL(logPriorityName, logPriorityChar, logOnStderr)                                \
void logVPrint##logPriorityName(const char* tag, const char* fmt, va_list args) {                  \
    printMessage(logPriorityChar, logOnStderr, tag, fmt, args);                                    \
}

ILOGGER_IMPL(Error, 'E', true)
ILOGGER_IMPL(Warn, 'W', true)
ILOGGER_IMPL(Info, 'I', false)
ILOGGER_IMPL(Debug, 'D', false)
ILOGGER_IMPL(Verbose, 'V', false)

#undef ILOGGER_IMPL





void logMessage(int logPriority, const char* tag, const char* fmt, va_list args) {
    char tag_buffer[LOGGER_TAG_MAX_LENGTH];

    if (logPriority == LOG_PRIORITY__ERROR && sCurrentLogLevel >= LOG_LEVEL__NORMAL) {
        logVPrintError(getFullLogTag(tag, tag_buffer, sizeof(tag_buffer)), fmt, args);
    } else if (logPriority == LOG_PRIORITY__WARN && sCurrentLogLevel >= LOG_LEVEL__NORMAL) {
        logVPrintWarn(getFullLogTag(tag, tag_buffer, sizeof(tag_buffer)), fmt, args);
    } else if (logPriority == LOG_PRIORITY__INFO && sCurrentLogLevel >= LOG_LEVEL__NORMAL) {
        logVPrintInfo(getFullLogTag(tag, tag_buffer, sizeof(tag_buffer)), fmt, args);
    } else if (logPriority == LOG_PRIORITY__DEBUG && sCurrentLogLevel >= LOG_LEVEL__DEBUG) {
        logVPrintDebug(getFullLogTag(tag, tag_buffer, sizeof(tag_buffer)), fmt, args);
    } else if (logPriority == LOG_PRIORITY__VERBOSE && sCurrentLogLevel >= LOG_LEVEL__VERBOSE) {
        logVPrintVerbose(getFullLogTag(tag, tag_buffer, sizeof(tag_buffer)), fmt, args);
    } else if (logPriority == LOG_PRIORITY__VVERBOSE && sCurrentLogLevel >= LOG_LEVEL__VVERBOSE) {
        logVPrintVerbose(getFullLogTag(tag, tag_buffer, sizeof(tag_buffer)), fmt, args);
    } else if (logPriority == LOG_PRIORITY__VVVERBOSE && sCurrentLogLevel >= LOG_LEVEL__VVVERBOSE) {
        logVPrintVerbose(getFullLogTag(tag, tag_buffer, sizeof(tag_buffer)), fmt, args);
    }
}



/**
 * Get full log tag to use for logging for a `tag`.
 *
 * If `tag` equals `sDefaultLogTag` or `sLogTagPrefix` is empty, then
 * `tag` is returned as is, otherwise `sLogTagPrefix` is prefixed
 * before `tag`.
 * If `tag` is `null` or empty, then `sDefaultLogTag` is returned.
 */
static const char* getFullLogTag(const char* tag, char *buffer, size_t buffer_len) {
    if (tag == NULL || strlen(tag) < 1) {
        return sDefaultLogTag;
    } else if (strcmp(sDefaultLogTag, tag) == 0 || strlen(sLogTagPrefix) < 1) {
        return tag;
    } else {
        snprintf(buffer, buffer_len, "%s%s", sLogTagPrefix, tag);
        return buffer;
    }
}





#define LOG_MESSAGE_IMPL(logPriorityName, logPriorityNum, logLevel)                                \
void log##logPriorityName(const char* tag, const char* fmt, ...) {                                 \
    if (sCurrentLogLevel >= logLevel) {                                                            \
        va_list args;                                                                              \
        va_start(args, fmt);                                                                       \
        logMessage(logPriorityNum, tag, fmt, args);                                                \
        va_end(args);                                                                              \
    }                                                                                              \
}

LOG_MESSAGE_IMPL(Error, LOG_PRIORITY__ERROR, LOG_LEVEL__NORMAL)
LOG_MESSAGE_IMPL(Warn, LOG_PRIORITY__WARN, LOG_LEVEL__NORMAL)
LOG_MESSAGE_IMPL(Info, LOG_PRIORITY__INFO, LOG_LEVEL__NORMAL)
LOG_MESSAGE_IMPL(Debug, LOG_PRIORITY__DEBUG, LOG_LEVEL__DEBUG)
LOG_MESSAGE_IMPL(Verbose, LOG_PRIORITY__VERBOSE, LOG_LEVEL__VERBOSE)
LOG_MESSAGE_IMPL(VVerbose, LOG_PRIORITY__VVERBOSE, LOG_LEVEL__VVERBOSE)
LOG_MESSAGE_IMPL(VVVerbose, LOG_PRIORITY__VVVERBOSE, LOG_LEVEL__VVVERBOSE)

#undef LOG_MESSAGE_IMPL





static bool logErrorForLogLevel(int logLevel, const char* tag, const char* fmt, va_list args) {
    if (sCurrentLogLevel >= logLevel) {
        logMessage(LOG_PRIORITY__ERROR, tag, fmt, args);
        return true;
    }
    return false;
}



#define LOG_ERROR_MESSAGE_FOR_LOG_LEVEL_IMPL(logPriorityName, logLevel)                            \
bool logError##logPriorityName(const char* tag, const char* fmt, ...) {                            \
    if (sCurrentLogLevel >= logLevel) {                                                            \
        va_list args;                                                                              \
        va_start(args, fmt);                                                                       \
        bool logged = logErrorForLogLevel(logLevel, tag, fmt, args);                               \
        va_end(args);                                                                              \
        return logged;                                                                             \
    }                                                                                              \
    return false;                                                                                  \
}

LOG_ERROR_MESSAGE_FOR_LOG_LEVEL_IMPL(Debug, LOG_LEVEL__DEBUG)
LOG_ERROR_MESSAGE_FOR_LOG_LEVEL_IMPL(Verbose, LOG_LEVEL__VERBOSE)
LOG_ERROR_MESSAGE_FOR_LOG_LEVEL_IMPL(VVerbose, LOG_LEVEL__VVERBOSE)
LOG_ERROR_MESSAGE_FOR_LOG_LEVEL_IMPL(VVVerbose, LOG_LEVEL__VVVERBOSE)
LOG_ERROR_MESSAGE_FOR_LOG_LEVEL_IMPL(Private, LOG_LEVEL__DEBUG)

#undef LOG_ERROR_MESSAGE_FOR_LOG_LEVEL_IMPL





void logStrerrorMessage(int errnoCode, const char* tag, const char* fmt, va_list args) {
    if (errnoCode == 0) {
        logMessage(LOG_PRIORITY__ERROR, tag, fmt, args);
    } else {
        char strerrorBuffer[STRERROR_BUFFER_SIZE];
        safe_strerror_r(errnoCode, strerrorBuffer, sizeof(strerrorBuffer));

        size_t strerror_length = strlen(strerrorBuffer);

        char currentMessage[LOGGER_ENTRY_MAX_PAYLOAD - strerror_length - 1];
        vsnprintf(currentMessage, sizeof(currentMessage), fmt, args);

        char newMessage[strlen(currentMessage) + strerror_length + 2 + 1];
        snprintf(newMessage, sizeof(newMessage), "%s: %s", currentMessage, strerrorBuffer);

        logError(tag, "%s", newMessage);
    }
}

void logStrerror(const char* tag, const char* fmt, ...) {
    int errnoCode = errno;
    va_list args;
    va_start(args, fmt);
    logStrerrorMessage(errnoCode, tag, fmt, args);
    va_end(args);
    errno = errnoCode;
}



#define LOG_STRERROR_MESSAGE_FOR_LOG_LEVEL_IMPL(logPriorityName, logLevel)                         \
bool logStrerror##logPriorityName(const char* tag, const char* fmt, ...) {                         \
    if (sCurrentLogLevel >= logLevel) {                                                            \
        int errnoCode = errno;                                                                     \
        va_list args;                                                                              \
        va_start(args, fmt);                                                                       \
        logStrerrorMessage(errnoCode, tag, fmt, args);                                             \
        va_end(args);                                                                              \
        errno = errnoCode;                                                                         \
        return true;                                                                               \
    }                                                                                              \
    return false;                                                                                  \
}

LOG_STRERROR_MESSAGE_FOR_LOG_LEVEL_IMPL(Debug, LOG_LEVEL__DEBUG)
LOG_STRERROR_MESSAGE_FOR_LOG_LEVEL_IMPL(Verbose, LOG_LEVEL__VERBOSE)
LOG_STRERROR_MESSAGE_FOR_LOG_LEVEL_IMPL(VVerbose, LOG_LEVEL__VVERBOSE)
LOG_STRERROR_MESSAGE_FOR_LOG_LEVEL_IMPL(VVVerbose, LOG_LEVEL__VVVERBOSE)
LOG_STRERROR_MESSAGE_FOR_LOG_LEVEL_IMPL(Private, LOG_LEVEL__DEBUG)

#undef LOG_STRERROR_MESSAGE_FOR_LOG_LEVEL_IMPL
