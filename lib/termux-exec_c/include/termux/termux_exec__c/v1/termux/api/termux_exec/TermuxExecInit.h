#ifndef LIBTERMUX_EXEC__C__TERMUX_EXEC_INIT___H
#define LIBTERMUX_EXEC__C__TERMUX_EXEC_INIT___H

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif



int initProcess(bool shouldLogVersion);
int initLogger(bool shouldLogVersion);
void setIgnoreExit(bool state);
int exitProcess();



#ifdef __cplusplus
}
#endif

#endif // LIBTERMUX_EXEC__C__TERMUX_EXEC_INIT___H
