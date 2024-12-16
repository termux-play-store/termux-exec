#ifndef TERMUX_EXEC_INIT_H
#define TERMUX_EXEC_INIT_H

#include <stdbool.h>

int initProcess(bool shouldLogVersion);
int initLogger(bool shouldLogVersion);
int exitProcess();

#endif // TERMUX_EXEC_INIT_H
