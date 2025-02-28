#ifndef LIBTERMUX_EXEC__C__TERMUX_EXEC_LIBRARY_CONFIG___H
#define LIBTERMUX_EXEC__C__TERMUX_EXEC_LIBRARY_CONFIG___H

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif



/** Get `sIsRunningTests`. */
bool libtermux_exec__c__getIsRunningTests();

/** Set `sIsRunningTests`. */
void libtermux_exec__c__setIsRunningTests(bool isRunningTests);



#ifdef __cplusplus
}
#endif

#endif // LIBTERMUX_EXEC__C__TERMUX_EXEC_LIBRARY_CONFIG___H
