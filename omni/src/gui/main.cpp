#include <QApplication>

#include "mainwindow.h"
#include "system/omFilter2d.h"
#include "system/omProject.h"
#include <dlfcn.h>
#include "system/omDebug.h"

GGOCTFPointer GGOCTFunction = 0;
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

int main(int argc, char *argv[])
{
	//    return firsttime (argc, argv);
	int fileArgIndex = parseAnythingYouCan(argc, argv);
	QApplication app(argc, argv);
	Q_INIT_RESOURCE(resources);
	MainWindow mainWin;
	mainWin.show();

	if (fileArgIndex) {
		mainWin.openProject(argv[fileArgIndex]);
	}

	return app.exec();
}

