#ifndef STANDARD_LOGGER_IMPL_H
#define STANDARD_LOGGER_IMPL_H

#include <stdbool.h>

#include "logger.h"

extern const struct ILogger sStandardLoggerImpl;

void printMessageToStdStream(bool logOnStderr, const char* message);

#endif // STANDARD_LOGGER_IMPL_H
