#ifndef FILE_LOGGER_IMPL_H
#define FILE_LOGGER_IMPL_H

#include <stdbool.h>

#include "logger.h"

extern const struct ILogger sFileLoggerImpl;

void printMessageToFile(bool logOnStderr, const char* message);

FILE* getLogFileStream();
const char* getLogFilePath();
int setLogFilePath(const char* logFilePath);
int closeLogFile();

#endif // FILE_LOGGER_IMPL_H
