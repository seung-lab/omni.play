#include <QApplication>

#include "mainwindow.h"
#include "volume/omFilter2d.h"
#include "project/omProject.h"
#include <dlfcn.h>
#include "common/omDebug.h"
#include <QTextStream>
#include "volume/omVolume.h"

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

OmId segmentationID = 0;

void processHeadlessLine( QString line, QString fileNameAndPath )
{
	if( line == "q" || line == "quit" ){
		printf("exiting...\n");
		exit(0);
	} else if( line.startsWith("mesh") ) {
		if( segmentationID > 0 ){
			printf("running mesh on segmentation %d\n", segmentationID);
			OmVolume::GetSegmentation( segmentationID ).BuildMeshData();
		}
	} else if( line.startsWith("seg") ) {
		QStringList args = line.split(':');
		bool ok;
		segmentationID = args[1].toUInt(&ok, 10); 
		if( ok ){
			printf("segmentationID set to %d\n", segmentationID );
		}
	} else if( line.startsWith("open") ){
		QString fname = fileNameAndPath.section('/', -1);
		QString dpath = fileNameAndPath.remove(fname);

		try {
			OmProject::Load(dpath.toStdString(), fname.toStdString());
			printf("opened project\n");
		} catch(...) {
			printf("could not open project\n");
		}

	
	} else {
		printf("could not parse \"%s\"\n", qPrintable(line) );
	}
}

void runHeadless( QString fname )
{
	QTextStream stream(stdin);
	QString line;
	do {
		line = stream.readLine();
		processHeadlessLine( line, fname );
	} while (!line.isNull());
}

int main(int argc, char *argv[])
{
	//    return firsttime (argc, argv);

	CmdLineArgs args = parseAnythingYouCan(argc, argv);
	if ( args.fileArgIndex < 0){
		return 0;
	}

	// args.headlessCMD is "--headless=WhateverYouPass"

	if( args.runHeadless ){
		printf("running headless...\n");
		if ( args.fileArgIndex > 0 ) {
			QString fname = QString::fromStdString( argv[ args.fileArgIndex ] );			
			runHeadless( fname );
			// runHeadless( args.headlessCMD );
		} else {
			runHeadless( "");
		}
	} else {
		QApplication app(argc, argv);
		Q_INIT_RESOURCE(resources);
		MainWindow mainWin;
		mainWin.show();
		
		if ( args.fileArgIndex > 0 ) {
			mainWin.openProject( argv[ args.fileArgIndex ]);
		}
		
		return app.exec();
	}
}

