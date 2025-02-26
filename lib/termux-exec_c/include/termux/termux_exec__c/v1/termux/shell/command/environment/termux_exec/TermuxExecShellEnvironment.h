#ifndef LIBTERMUX_EXEC__C__TERMUX_EXEC_SHELL_ENVIRONMENT___H
#define LIBTERMUX_EXEC__C__TERMUX_EXEC_SHELL_ENVIRONMENT___H

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif



/*
 * Environment for `termux-exec`.
 */

/**
 * Environment variable for the log level for `termux-exec`.
 *
 * Type: `int`
 * Default key: `TERMUX_EXEC__LOG_LEVEL`
 * Default value: DEFAULT_LOG_LEVEL
 * Values:
 * - `0` (`OFF`) - Log nothing.
 * - `1` (`NORMAL`) - Log error, warn and info messages and stacktraces.
 * - `2` (`DEBUG`) - Log debug messages.
 * - `3` (`VERBOSE`) - Log verbose messages.
 * - `4` (`VVERBOSE`) - Log very verbose messages.
 */
#define ENV__TERMUX_EXEC__LOG_LEVEL TERMUX_ENV__S_TERMUX_EXEC "LOG_LEVEL"



/**
 * Environment variable for whether `termux-exec` should intercept
 * `execve()` wrapper [declared in `unistd.h`.
 *
 * Type: `bool`
 * Default key: `TERMUX_EXEC__INTERCEPT_EXECVE`
 * Default value: E_DEF_VAL__TERMUX_EXEC__INTERCEPT_EXECVE
 * Values:
 * - `true` - Intercept `execve()` enabled.
 * - `false` - Intercept `execve()` disabled.
 */
#define ENV__TERMUX_EXEC__INTERCEPT_EXECVE TERMUX_ENV__S_TERMUX_EXEC "INTERCEPT_EXECVE"
static const bool E_DEF_VAL__TERMUX_EXEC__INTERCEPT_EXECVE = true;

/**
 * Environment variable for whether use System Linker Exec Solution,
 * like to bypass App Data File Execute Restrictions.
 *
 * See also `shouldSystemLinkerExec()`
 *
 * Type: `string`
 * Default key: `TERMUX_EXEC__SYSTEM_LINKER_EXEC`
 * Default value: E_DEF_VAL__TERMUX_EXEC__SYSTEM_LINKER_EXEC
 * Values:
 * - `disable` - The `system_linker_exec` will be disabled.
 * - `enable` - The `system_linker_exec` will be enabled but only if required.
 * - `force` - The `system_linker_exec` will be force enabled even if not required.
 */
#define ENV__TERMUX_EXEC__SYSTEM_LINKER_EXEC TERMUX_ENV__S_TERMUX_EXEC "SYSTEM_LINKER_EXEC"
static const int E_DEF_VAL__TERMUX_EXEC__SYSTEM_LINKER_EXEC = 1;



/**
 * Environment variable for the path to the executable file is being
 * executed by `execve()` is using `system_linker_exec`.
 *
 * Type: `string`
 * Default key: `TERMUX_EXEC__PROC_SELF_EXE`
 * Values:
 * - The normalized, absolutized and prefixed path for the executable
 * file is being executed by `execve()` if `system_linker_exec` is
 * being used.
 */
#define ENV__TERMUX_EXEC__PROC_SELF_EXE TERMUX_ENV__S_TERMUX_EXEC "PROC_SELF_EXE"
#define ENV_PREFIX__TERMUX_EXEC__PROC_SELF_EXE ENV__TERMUX_EXEC__PROC_SELF_EXE "="





/**
 * Returns the `termux-exec` config for `Logger` log level
 * based on the `ENV__TERMUX_EXEC__LOG_LEVEL` env variable.
 *
 * @return Return the value if `ENV__TERMUX_EXEC__LOG_LEVEL` is
 * set, otherwise defaults to `DEFAULT_LOG_LEVEL`.
 */
int getTermuxExecLogLevel();



/**
 * Returns the `termux-exec` config for whether `execve` should be
 * intercepted based on the `ENV__TERMUX_EXEC__INTERCEPT_EXECVE` env variable.
 *
 * See also `getBoolEnvValue()`.
 *
 * @return Return `true` if `ENV__TERMUX_EXEC__INTERCEPT_EXECVE` is
 * bool enabled, `false` if bool disabled, otherwise defaults to `true`.
 */
bool isTermuxExecExecveInterceptEnabled();

/**
 * Returns the `termux-exec` config for `system_linker_exec` based on
 * the `ENV__TERMUX_EXEC__SYSTEM_LINKER_EXEC` env variable.
 *
 * @return Return `0` if `ENV__TERMUX_EXEC__SYSTEM_LINKER_EXEC` is set
 * to `disable`, `1` if set to `enable`, `2` if set to `force`,
 * otherwise defaults to `1` (`enable`).
 */
int getTermuxExecSystemLinkerExecConfig();



#ifdef __cplusplus
}
#endif

#endif // LIBTERMUX_EXEC__C__TERMUX_EXEC_SHELL_ENVIRONMENT___H
