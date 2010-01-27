#include <pthread.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include "system/omDebug.h"

//***********************************************************************
// TODO Convert to command line switches ---> get rid of #defs
//      - Make this a singleton class
//      - Include a ParseArg function
//      - Include a Default Settings Section
//      - Command Line input will override default
//***********************************************************************

//***********************************************************************
//****  SET YOUR DEF FLAGS HERE  (temporary)               
//***********************************************************************
#define OM_CACHE           0	// Testing of the Cache
#define OM_TESTRUN         0	// Gen One's Old Debugs
#define OM_MUTEX           0	// pthread_mutexes - all mutexes noted
#define OM_MESHER          1
#define OM_GENONE          0
#define OM_THREAD          1
//*********************************************************************** 
//***********************************************************************

/** Your general debug function . . . it prints to stdout */
void debug(const char *category, const char *format, ...)
{
	va_list args;
	va_start(args, format);

//----------------------------------------------        
	if (!strcmp(category, "cache")) {
#if OM_CACHE
		vfprintf(stdout, format, args);
#endif
	}
//----------------------------------------------
	if (!strcmp(category, "testrun")) {
#if OM_TESTRUN
		vfprintf(stdout, format, args);
#endif
	}
//----------------------------------------------
	if (!strcmp(category, "mesher")) {
#if OM_MESHER
		vfprintf(stdout, format, args);
#endif
	}
	va_end(args);
//----------------------------------------------
	if (!strcmp(category, "genone")) {
#if OM_GENONE
		vfprintf(stdout, format, args);
#endif
	}
//----------------------------------------------
	if (!strcmp(category, "mutex")) {
#if OM_MUTEX
		vfprintf(stdout, format, args);
#endif
	}
//----------------------------------------------
	if (!strcmp(category, "thread")) {
#if OM_THREAD
		vfprintf(stdout, format, args);
#endif
	}
//----------------------------------------------

	va_end(args);
}
