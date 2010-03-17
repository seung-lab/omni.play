#include <signal.h>
#include <execinfo.h>
#include <dlfcn.h>

#include "mainwindow.h"
#include "headless.h"
#include "project/omProject.h"

GGOCTFPointer GGOCTFunction = 0;

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
       	void *array[1000];
       	size_t size;
       	char **strings;
       	size_t i;
     
       	size = backtrace (array, 1000);
       	strings = backtrace_symbols (array, size);
     
       	fprintf (stderr, "Obtained %zd stack frames.\n", size);
     
	for (i = 0; i < size; i++)
		fprintf (stderr, "%s\n", strings[i]);
     
	free (strings);

	exit(0);
}

int main(int argc, char *argv[])
{
	signal (SIGSEGV, myBacktrace);
	signal (SIGBUS, myBacktrace);
	signal (SIGABRT, myBacktrace);

	Headless headless;
	return headless.start(argc, argv);
}

