#include <signal.h>

#include "headless.h"

#ifndef WIN32
#include <execinfo.h>
#include <dlfcn.h>
#endif


//GGOCTFPointer GGOCTFunction = 0;

/*
int firsttime(int argc, char *argv[])
{
	switch (argc) {
	case 3:
	case 2:
		void *lp = dlopen(argv[1], RTLD_LAZY);
		if (lp) {
			GGOCTFunction = (GGOCTFPointer) dlsym(lp, "omnichanneltile");
		}
		break;
	}

	QApplication app(argc, argv);
	Q_INIT_RESOURCE(resources);
	MainWindow mainWin;
	mainWin.show();

	switch (argc) {
	case 3:
		mainWin.openProject(argv[2]);
		break;
	}

	return app.exec();
}
*/

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

