#define _GNU_SOURCE
#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>

#include "logger.h"
#include "standard_logger_impl.h"

const struct ILogger sStandardLoggerImpl = {
    .printMessage = printMessageToStdStream,
};



void printMessageToStdStream(bool logOnStderr, const char *message) {
    fprintf(logOnStderr ? stderr : stdout, "%s", message);

    if (!logOnStderr) {
        fflush(stdout);
    }
}
