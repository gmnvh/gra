#ifndef MAIN_H
#define MAIN_H

#include "trace.h"

/**
 * Configuration macros
 */

/**
 * Help string to be showed as help context for the console command
 */
#define OPT_HELP "\r\nGRA [-i]\r\n\r\n" \
				 "i\tInput type\r\n"

/**
 * Types
 */

/**
 * Input types
 */
typedef enum mainTInputEnum {
	INPUT_WEBCAM = 0,
	INPUT_PHOTO,
	INPUT_VIDEO
} mainTInput;

/**
 * Options configured by command parameters
 */
typedef struct mainOptionStruct {
	traceLevel trace;				/* Trace level */
	mainTInput input;				/* Input */
	char *inputFile;				/* Input files */
} mainTOption;
#define mainTOptionInit() {TRACE_LV_DEBUG, INPUT_WEBCAM, NULL}

#endif
