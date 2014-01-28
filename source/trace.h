#ifndef TRACE_H
#define TRACE_H

/**
 * When using this header is important define some macros in the source code:
 *
 * THIS_MODULE "MODULE_NAME" -> Module name to be showed when printing the trace message
 *
 * THIS_LEVEL mainLevel -> traceLevel type variable with the level configuration (debug, info,
 * 						   warning or error)
 *
 * 	Example:
 *
 * 	\code
 *
 * 	#include <stdio>
 *
 * 	#define THIS_MODULE "MAIN"
 * 	#define THIS_LEVEL gLevel
 * 	#include "trace.h"
 *
 * 	void f(void)
 * 	{
 * 		TRACE_INFO("Example");
 *  }
 *
 * 	\endcode
 */

/**
 * Configuration macros
 */


/**
 * Types
 */

/**
 * Trace levels
 */
typedef enum {
	TRACE_LV_DEBUG,
	TRACE_LV_INFO,
	TRACE_LV_WARNING,
	TRACE_LV_ERROR
} traceLevel;

/**
 * Macros
 */

/** Helper macro for internal use. */
#define TRACE_BLDMSG(_level, _msg) "<" THIS_MODULE "," _level "> " _msg "\r\n"

/** Macro that prints the trace message */
#define TRACE_MSG(_level, _msg, ...) printf(TRACE_BLDMSG(_level, _msg), ##__VA_ARGS__);

/** Debug trace */
#define TRACE_DEBUG(_msg, ...) if((THIS_LEVEL) <= TRACE_LV_DEBUG) {TRACE_MSG("D", _msg, ##__VA_ARGS__)}

/** Info trace */
#define TRACE_INFO(_msg, ...) if((THIS_LEVEL) <= TRACE_LV_INFO) {TRACE_MSG("I", _msg, ##__VA_ARGS__)}

/** Warning trace */
#define TRACE_WARN(_msg, ...) if((THIS_LEVEL) <= TRACE_LV_WARNING) {TRACE_MSG("W", _msg, ##__VA_ARGS__)}

/** Error trace */
#define TRACE_ERROR(_msg, ...) if((THIS_LEVEL) <= TRACE_LV_ERROR) {TRACE_MSG("E", _msg, ##__VA_ARGS__)}


/** Debug trace to be used in periodic functions */
#define TRACE_PR_DEBUG(_freq, _msg, ...) {static int _c = 0;_c++;if((_c%_freq)==0){TRACE_DEBUG(_msg, ##__VA_ARGS__);}}

#endif
