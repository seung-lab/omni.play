#include <QApplication>

#include <dlfcn.h>

#include <QTextStream>
#include <QFile>
#include <QFileInfo>
#include <QTime>
#include <time.h>

#include "mainwindow.h"
#include "volume/omFilter2d.h"
#include "project/omProject.h"
#include "volume/omVolume.h"
#include "common/omDebug.h"
#include "system/omGarbage.h"
#include "gui/inspectors/mutexServer.h"

OmId SegmentationID = 0;

GGOCTFPointer GGOCTFunction = 0;
int argc_global;
char **argv_global;

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

void openProject( QString fName )
{
	try {
		printf("please wait: opening project \"%s\"...\n", qPrintable( fName ));
		OmProject::Load( fName );
		printf("opened project \"%s\"\n", qPrintable( fName ));
	} catch(...) {
	        printf("error while loading project \"%s\"\n", qPrintable( fName ));
	}
}

unsigned int getNum( QString arg )
{
	bool ok;
	unsigned int ret = arg.toUInt(&ok, 10); 
	if( ok ){
		return ret;
	} else {
		printf("could not parse to unsigned int \"%s\"\n", qPrintable(arg) );
		return 0;
	}
}

void processLine( QString line, QString fName )
{
	time_t start;
	time_t end;
	double dif;

	if( line == "q" || line == "quit" ){
		printf("exiting...\n");
		exit(0);
	} else if( "save" == line ) {
		OmProject::Save();
	} else if( "mesh" == line ) {
		if( 0 == SegmentationID  ){
			printf("please choose segmentation first!\n");
			return;
		} 

		printf("running mesh on segmentation %d\n", SegmentationID);


		time (&start);
		OmVolume::GetSegmentation( SegmentationID ).BuildMeshData();
		time (&end);
		dif = difftime (end,start);
		printf("meshing done (%.2lf secs)\n", dif );

	} else if( line.startsWith("meshchunk") ) {
		// format: meshchunk:segmentationID:mipLevel:x,y,z
		QStringList args = line.split(':');
		SegmentationID = getNum( args[1] );
		unsigned int mipLevel = getNum( args[2] );
		QStringList coords = args[3].split(',');
		unsigned int x = getNum( coords[0] );
		unsigned int y = getNum( coords[1] );
		unsigned int z = getNum( coords[2] );
		
		int numThreads=0;
		if( 5 == args.size() ){
			numThreads = getNum( args[4] );
			printf("over-road edfault number of threads...\n");
		}
		printf("meashing chunk %d, %d, %d, %d...", mipLevel, x, y, z );
		time (&start);
		OmVolume::GetSegmentation( SegmentationID ).BuildMeshChunk( mipLevel, x, y, z, numThreads);
		time (&end);
		dif = difftime (end,start);
		printf("meshing done (%.2lf secs)\n", dif );
	} else if( "meshplan" == line ) {
		if( 0 == SegmentationID  ){
			printf("please choose segmentation first!\n");
			return;
		} 
		QString planFile = fName + ".plan";
		OmVolume::GetSegmentation( SegmentationID ).BuildMeshDataPlan(planFile);
		printf("wrote plan to \"%s\"\n", qPrintable( planFile ) );
	} else if( "meshdone" == line ) {
		if( 0 == SegmentationID  ){
			printf("please choose segmentation first!\n");
			return;
		} 
		OmVolume::GetSegmentation( SegmentationID ).mMipMeshManager.SetMeshDataBuilt(true);
		OmProject::Save();
	} else if( line.startsWith("seg") ) {
		QStringList args = line.split(':');
		SegmentationID = getNum( args[1] );
		if( SegmentationID > 0 ){
			printf("segmentationID set to %d\n", SegmentationID );
		} else {
			printf("invalid segmentation\n");
		}
	} else if( line.startsWith("openFile") ){
		QStringList args = line.split(':');
		openProject( args[1] );
	} else if( line.startsWith("open") ){
		openProject( fName );
	} else if( line.startsWith("parallel") ){
		QStringList args = line.split(':');
		OmGarbage::SetParallel(args[1], getNum(args[2]));
	} else if( line.startsWith("serve") ){
		QStringList args = line.split(':');
		QCoreApplication app(argc_global, argv_global);

		MutexServer * server = new MutexServer (args[1], getNum(args[2]));
		server->start ();
		
		app.exec ();
	} else {
		printf("could not parse \"%s\"\n", qPrintable(line) );
	}
}

void runInteractive( QString fName )
{
	QTextStream stream(stdin);
	QString line;
	do {
		line = stream.readLine();
		processLine( line, fName );
	} while (!line.isNull());
}

void runScript( const QString scriptFileName, QString fName )
{
	if (!QFile::exists( scriptFileName )) {
		printf("could not open plan file \"%s\"\n", qPrintable(scriptFileName));
		exit(1);
	}
		
	QFile file(scriptFileName);
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
		printf("could not read plan file \"%s\"\n", qPrintable(scriptFileName));
		exit(1);
	}
		
	QTextStream in(&file);
	while (!in.atEnd()) {
		QString line = in.readLine();
		processLine( line, fName );
	}
}

void runHeadless( QString headlessCMD, QString fName )
{	

	if( fName != "" ){
		openProject( fName );
	}

	if( "--headless" == headlessCMD ){
		runInteractive( fName );
	} else {
		QString planFileName = headlessCMD;
		planFileName.replace("--headless=", "");

		runScript( planFileName, fName );
	}
}

void setOmniExecutablePath( QString rel_fnpn )
{
	QFileInfo fInfo(rel_fnpn);
	QString fnpn = fInfo.absoluteFilePath();

	OmStateManager::setOmniExecutableAbsolutePath( fnpn );
}

int main(int argc, char *argv[])
{
	argc_global = argc;
	argv_global = argv;
	//    return firsttime (argc, argv);

	CmdLineArgs args = parseAnythingYouCan(argc, argv);
	if ( args.fileArgIndex < 0){
		return 0;
	}

	QString fName = "";
	if ( args.fileArgIndex > 0 ) {
		fName = QString::fromStdString( argv[ args.fileArgIndex ] );
	}

	if( args.runHeadless ){
		runHeadless( args.headlessCMD, fName );
	} else {
		setOmniExecutablePath( QString( argv[0] ) );
		QApplication app(argc, argv);
		Q_INIT_RESOURCE(resources);
		MainWindow mainWin;
		mainWin.show();
		
		if ( args.fileArgIndex > 0 ) {
			mainWin.openProject( fName );
		}
		
		return app.exec();
	}
}

