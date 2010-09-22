#include <signal.h>
#include "headless.h"
#include <zi/args>

#ifndef WIN32
#include <execinfo.h>
#include <dlfcn.h>
#endif


ZiARG_bool(      someBool,        true, "Sample bool argument");
ZiARG_bool(   anotherBool,       false, "Another bool argument");
ZiARG_int32(    someInt32,           0, "Sample int32 argument");
ZiARG_int64(    someInt64,           1, "Sample int64 argument");
ZiARG_uint64(  someUInt64,           2, "Sample uint64 argument");
ZiARG_double(  someDouble,         3.0, "Sample double argument");
ZiARG_float(    someFloat,         4.0, "Sample float argument");
ZiARG_string(  someString,  "string_5", "Sample string argument");

//int backtrace (void **buffer, int size);
//char ** backtrace_symbols (void *const *buffer, int size);
void myBacktrace (int sig)
{
#ifndef WIN32
       	void *array[1000];
       	size_t size;
       	char **strings;
       	size_t i;
	FILE * out = stderr;

	if(!sig) {
		out = stdout;
	}

       	size = backtrace (array, 1000);
       	strings = backtrace_symbols (array, size);

       	//fprintf (stderr, "Obtained %zd stack frames.\n", size);
       	fprintf (out, "Obtained %zd stack frames.\n", size);

	for (i = 0; i < size; i++)
		fprintf (out, "%s\n", strings[i]);

	free (strings);

	if(sig) {
		exit(0);
	}
#endif
}

#ifdef WIN32
void * __gxx_personality_v0=0;
void * _Unwind_Resume =0;
#endif

int main(int argc, char *argv[])
{
#ifndef WIN32
	signal (SIGSEGV, myBacktrace);
	signal (SIGBUS, myBacktrace);
	signal (SIGABRT, myBacktrace);
	signal (SIGFPE, myBacktrace);
	signal (SIGILL, myBacktrace);
#endif

	Headless headless;
	return headless.start(argc, argv);
}

