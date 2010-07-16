#include <QApplication>
#include <QTextStream>
#include <QFile>
#include <QDir>
#include <QFileInfo>
#include <QTime>
#include <QProcess>
#include <time.h>

#include "volume/omMipChunk.h"
#include "common/omDebug.h"
#include "datalayer/omDataLayer.h"
#include "datalayer/omDataPaths.h"
#include "gui/headless.h"
#include "gui/mainwindow.h"
#include "gui/recentFileList.h"
#include "project/omProject.h"
#include "project/omProjectSaveAction.h"
#include "segment/omSegmentCache.h"
#include "system/omBuildChannel.h"
#include "system/omBuildSegmentation.h"
#include "system/omLocalPreferences.h"
#include "system/omProjectData.h"
#include "system/omStateManager.h"
#include "utility/stringHelpers.h"
#include "volume/omFilter2d.h"
#include "volume/omSegmentation.h"
#include "volume/omSegmentationChunkCoord.h"
#include "volume/omVolume.h"

int argc_global;
char **argv_global;

OmId SegmentationID = 0;

void Headless::openProject( QString fName )
{
	try {
		printf("Please wait: Opening project \"%s\"...\n", qPrintable( fName ));
		OmProject::Load( fName );
		printf("Opened project \"%s\"\n", qPrintable( fName ));
	} catch(...) {
	        printf("Error while loading project \"%s\"\n", qPrintable( fName ));
	}
}

void doMeshinate( OmSegmentation * current_seg )
{
        QString abs_fnpn = OmProjectData::getAbsoluteFileNameAndPath();
        QString fnpnPlan = abs_fnpn + ".plan";
        current_seg->BuildMeshDataPlan( fnpnPlan );

	QString& GetScriptCmd (QString arg);
        QString script = GetScriptCmd (fnpnPlan);
        debug ("meshinator", "%s\n", qPrintable (script));

	//printf("here 2\n");

        QProcess * meshinatorProc = new QProcess ();
        meshinatorProc->start(script);
        meshinatorProc->waitForFinished(-1);

	//printf("here 3\n");

	(new OmProjectSaveAction())->Run();
}

void Headless::processLine( QString line, QString fName )
{
	OmTimer timer;

	if( line == "q" || line == "quit" ){
		printf("Exiting...\n");
		OmProject::Close();
		exit(0);
	} else if( "save" == line ) {
		(new OmProjectSaveAction())->Run();
	} else if( "mesh" == line ) {
		if( 0 == SegmentationID  ){
			printf("Please choose segmentation first!\n");
			return;
		} 
		OmSegmentation & added_segmentation = OmProject::GetSegmentation(SegmentationID);
		OmBuildSegmentation bs( &added_segmentation );
		bs.build_seg_mesh();
		bs.wait();
        } else if( "meshinator" == line ) {
                if( 0 == SegmentationID  ){
                        printf("Please choose segmentation first!\n");
                        return;
                }
                OmSegmentation & added_segmentation = OmProject::GetSegmentation(SegmentationID);
		(new OmProjectSaveAction())->Run();
		doMeshinate(&added_segmentation);
	} else if( "loadDend" == line ) {
		if( 0 == SegmentationID  ){
			printf("Please choose segmentation first!\n");
			return;
		} 
		OmSegmentation & added_segmentation = OmProject::GetSegmentation(SegmentationID);
		OmBuildSegmentation bs( &added_segmentation );
		bs.loadDendrogram();
	} else if( line.startsWith("compareChannels:") ) {
		// format: compareChannels:id1,id2[:verbose]
		QStringList args = line.split(':',QString::SkipEmptyParts);
		
		if ( args.size() < 2 ){
			printf("Please specify channel IDs.\n");
			return;
		}

		QStringList channelIDs = args[1].split(',',QString::SkipEmptyParts);

		if ( channelIDs.size() < 2 ){
			printf("Not enough channel IDs specified. Please specify as 'id1,id2'.\n");
			return;
		}

		int id1 = StringHelpers::getUInt( channelIDs[0] );
		int id2 = StringHelpers::getUInt( channelIDs[1] );

		bool verbose = 0;

		if ( 3 == args.size() ){
			verbose = (bool) StringHelpers::getUInt( args[2] );
		}

		if( !OmProject::IsChannelValid(id1) && !OmProject::IsChannelValid(id2) ) {
			if( id1 == id2 ){
				printf("Channel %i is not a valid channel.\n",id1);
			} else {
				printf("Channels %i and %i are not valid channels.\n",id1,id2);
			}
		} else if( !OmProject::IsChannelValid(id1) ) {
			printf("Channel %i is not a valid channel.\n",id1);
		} else if( !OmProject::IsChannelValid(id2) ) {
			printf("Channel %i is not a valid channel.\n",id2);
		} else if( OmMipVolume::CompareVolumes(&OmProject::GetChannel(id1),&OmProject::GetChannel(id2),verbose) ) {
			printf("Channel %i and Channel %i are identical.\n",id1,id2);
		}
	} else if( line.startsWith("compareSegmentations:") ) {
		// format: compareSegmentations:id1,id2[:verbose]
		QStringList args = line.split(':',QString::SkipEmptyParts);
		
		if ( args.size() < 2 ){
			printf("Please specify segmentation IDs.\n");
			return;
		}

		QStringList segmentationIDs = args[1].split(',',QString::SkipEmptyParts);

		if ( segmentationIDs.size() < 2 ){
			printf("Not enough segmentation IDs specified. Please specify as 'id1,id2'.\n");
			return;
		}

		int id1 = StringHelpers::getUInt( segmentationIDs[0] );
		int id2 = StringHelpers::getUInt( segmentationIDs[1] );

		bool verbose = 0;

		if ( 3 == args.size() ){
			verbose = (bool) StringHelpers::getUInt( args[2] );
		}

		if( !OmProject::IsSegmentationValid(id1) && !OmProject::IsSegmentationValid(id2) ) {
			if( id1 == id2 ){
				printf("Segmentation %i is not a valid channel.\n",id1);
			} else {
				printf("Segmentations %i and %i are not valid channels.\n",id1,id2);
			}
		} else if( !OmProject::IsSegmentationValid(id1) ) {
			printf("Segmentation %i is not a valid segmentation.\n",id1);
		} else if( !OmProject::IsSegmentationValid(id2) ) {
			printf("Segmentation %i is not a valid segmentation.\n",id2);
		} else if( OmMipVolume::CompareVolumes(&OmProject::GetSegmentation(id1),&OmProject::GetSegmentation(id2),verbose) ) {
			printf("Segmentation %i and Segmentation %i are identical.\n",id1,id2);
		}
	} else if( line.startsWith("meshchunk:") ) {
		// format: meshchunk:segmentationID:mipLevel:x,y,z[:numthreads]
		QStringList args = line.split(':',QString::SkipEmptyParts);
		
		if ( args.size() < 4 ){
			printf("Invalid format. Did you forget some arguments?\n");
			return;
		}

		SegmentationID = StringHelpers::getUInt( args[1] );
		unsigned int mipLevel = StringHelpers::getUInt( args[2] );
		QStringList coords = args[3].split(',',QString::SkipEmptyParts);

		if ( args.size() < 3 ){
			printf("Not enough coordinates specified. Please specify as 'x,y,z'.\n");
			return;
		}

		int x = StringHelpers::getUInt( coords[0] );
		int y = StringHelpers::getUInt( coords[1] );
		int z = StringHelpers::getUInt( coords[2] );

		OmMipMeshCoord mipChunkCoord = OmMipMeshCoord(OmMipChunkCoord(mipLevel, x, y, z), SegmentationID);
		OmDataWriter * hdf5File;

  		if (OmLocalPreferences::getStoreMeshesInTempFolder() || OmStateManager::getParallel()) {
			OmDataLayer * dl = OmProjectData::GetDataLayer();
    			hdf5File = dl->getWriter( QString::fromStdString( OmDataPaths::getLocalPathForHd5fChunk(mipChunkCoord, SegmentationID) ), false );
			hdf5File->create();
			hdf5File->open();
                }
		
		int numThreads=0;
		if( 5 == args.size() ){
			numThreads = StringHelpers::getUInt( args[4] );
			printf("Overrode default number of threads...\n");
		}
		if( 0 == numThreads) {
			numThreads = 4;
		}
		printf("Meshing chunk %d, %d, %d, %d...", mipLevel, x, y, z );
		timer.start();
		OmProject::GetSegmentation( SegmentationID ).BuildMeshChunk( mipLevel, x, y, z, numThreads);
		timer.stop();
		printf("Meshing done (%.6f secs)\n", timer.s_elapsed() );

		if (OmLocalPreferences::getStoreMeshesInTempFolder() ||
      			OmStateManager::getParallel()) {
    			hdf5File->close();
  		}
	} else if( line.startsWith("runMeshPlan") ) {
		timer.start();
		runMeshPlan( line );
		timer.stop();
		printf("Meshing done (%.6f secs)\n", timer.s_elapsed() );
	} else if( "meshplan" == line ) {
		if( 0 == SegmentationID  ){
			printf("Please choose segmentation first!\n");
			return;
		} 
		QString planFile = fName + ".plan";
		//		QString planFile = fName + QString(".seg%1.plan").arg(SegmentationID);
		OmProject::GetSegmentation( SegmentationID ).BuildMeshDataPlan(planFile);
		printf("Wrote plan to \"%s\"\n", qPrintable( planFile ) );
	} else if( line.startsWith("seg:") ) {
		QStringList args = line.split(':',QString::SkipEmptyParts);
		if ( args.size() < 2 ){
			printf("Please enter a filename.\n");
			return;
		}
		SegmentationID = StringHelpers::getUInt( args[1] );
		if( SegmentationID > 0 ){
			printf("SegmentationID set to %d\n", SegmentationID );
		} else {
			printf("Invalid segmentation\n");
		}
	} else if( line.startsWith("openFile:") ){
		QStringList args = line.split(':',QString::SkipEmptyParts);
		if ( args.size() < 2 ){
			printf("Please enter a filename.\n");
			return;
		}
		openProject( args[1] );
	} else if( line.startsWith("open") ){
		openProject( fName );
	} else if( line.startsWith("parallel") ){
		OmStateManager::setParallel(true);
        } else if( line.startsWith("addSegment") ){
                if( 0 == SegmentationID  ){
                        printf("Please choose segmentation first!\n");
                        return;
                }
                OmProject::GetSegmentation( SegmentationID ).GetSegmentCache()->AddSegment();
		(new OmProjectSaveAction())->Run();
	} else if( line.startsWith("create:") ) {
		QStringList args = line.split(':',QString::SkipEmptyParts);

		if ( args.size() < 2 ){
			printf("Please enter a filename.\n");
			return;
		}

		QString projectFileNameAndPath = args[1];      
                const QString fname = OmProject::New( projectFileNameAndPath );
		RecentFileList::prependFileToFS(fname);

	} else if( line.startsWith("createOrOpen:") ) {
		QStringList args = line.split(':',QString::SkipEmptyParts);

		if ( args.size() < 2 ){
			printf("Please enter a filename.\n");
			return;
		}

		QString projectFileNameAndPath = args[1];

		QFile file( projectFileNameAndPath );
		if(file.exists()){
			OmProject::Load( projectFileNameAndPath );
			
		} else {
			OmProject::New( projectFileNameAndPath );
		}
		
	} else if( line.startsWith("loadHDF5seg:") ){
		QStringList args = line.split(':',QString::SkipEmptyParts);

		if ( args.size() < 2 ){
			printf("Please enter a filename.\n");
			return;
		}

		OmSegmentation & added_segmentation = OmProject::AddSegmentation();
		SegmentationID = added_segmentation.GetId();
		QString hdf5fnp = args[1];

		OmBuildSegmentation bs( &added_segmentation );
		bs.addFileNameAndPath( hdf5fnp );
		bs.build_seg_image();
		bs.wait();
	} else if( line.startsWith("loadHDF5chann:") ){
		QStringList args = line.split(':',QString::SkipEmptyParts);
		
		if ( args.size() < 2 ){
			printf("Please enter a filename.\n");
			return;
		}

		OmChannel & chann = OmProject::AddChannel();
		QString hdf5fnp = args[1];

		OmBuildChannel bc( &chann );
		bc.addFileNameAndPath( hdf5fnp );
		bc.build_channel();
		bc.wait();
	} else if( line.startsWith("loadTIFFchann:") ){
		QStringList args = line.split(':',QString::SkipEmptyParts);

		if ( args.size() < 2 ){
			printf("Please enter a path to a directory.\n");
			return;
		}

		OmChannel & chann = OmProject::AddChannel();
		OmBuildChannel bc( &chann );

		QDir dir( args[1] );
		foreach( QFileInfo f, dir.entryInfoList() ){
			if(!f.isFile()){
				continue;
			}
			//printf("adding %s/\n", qPrintable( f.canonicalFilePath() ) );
			bc.addFileNameAndPath( f.canonicalFilePath() );
		}
		bc.build_channel();
		bc.wait();
        } else if( line.startsWith("loadTIFFseg:") ){
                QStringList args = line.split(':',QString::SkipEmptyParts);

		if ( args.size() < 2 ){
			printf("Please enter a path to a directory.\n");
			return;
		}

                OmSegmentation & seg = OmProject::AddSegmentation();
		SegmentationID = seg.GetId();
                OmBuildSegmentation bs( &seg );

                QDir dir( args[1] );
                foreach( QFileInfo f, dir.entryInfoList() ){
                        if(!f.isFile()){
                                continue;
                        }
                        //printf("adding %s/\n", qPrintable( f.canonicalFilePath() ) );
                        bs.addFileNameAndPath( f.canonicalFilePath() );
                }
                bs.build_seg_image();
                bs.wait();
	} else if( line.startsWith("buildHDF5:") ){
		QStringList args = line.split(':',QString::SkipEmptyParts);

		if ( args.size() < 2 ){
			printf("Please enter a filename.\n");
			return;
		}

		QString projectFileName = QFileInfo(args[1]+".omni").fileName();
		
		OmProject::New( projectFileName );

		OmSegmentation & added_segmentation = OmProject::AddSegmentation();
		QString hdf5fnp = args[1];

		OmBuildSegmentation bs( &added_segmentation );
		bs.addFileNameAndPath( hdf5fnp );
		bs.buildAndMeshSegmentation();
		bs.wait();
	} else if( line.startsWith("loadChunk") ){
		if( 0 == SegmentationID ){
                        printf("Please choose segmentation first!\n");
                        return;
                }
		
		OmSegmentation & segmen = OmProject::GetSegmentation(SegmentationID);
		OmMipChunkCoord chunk_coord(0,0,0,0);
		
		QExplicitlySharedDataPointer < OmMipChunk > p_chunk = QExplicitlySharedDataPointer < OmMipChunk > ();
		segmen.GetChunk(p_chunk, chunk_coord);
		p_chunk->Open();
        } else if( line.startsWith("removeChann:") ){
                QStringList args = line.split(':',QString::SkipEmptyParts);

		if ( args.size() < 2 ){
			printf("Please enter a channel id.\n");
			return;
		}

		int channID = StringHelpers::getUInt( args[1] );		

		if ( !OmProject::IsChannelValid(channID) ){
			printf("Channel %i is not a valid channel.\n",channID);
			return;
		} else {
			OmProject::RemoveChannel(channID);
			printf("Channel %i removed.\n",channID);
			return;
		}
	} else if( line.startsWith("removeSeg:") ){
                QStringList args = line.split(':',QString::SkipEmptyParts);

		if ( args.size() < 2 ){
			printf("Please enter a segmentation id.\n");
			return;
		}

		int segID = StringHelpers::getUInt( args[1] );		

		if ( !OmProject::IsSegmentationValid(segID) ){
			printf("Segmentation %i is not a valid segmentation.\n",segID);
			return;
		} else {
			OmProject::RemoveSegmentation(segID);
			printf("Segmentation %i removed.\n",segID);
			return;
		}
        } else {
		printf("Could not parse \"%s\"\n", qPrintable(line) );
	}
}

void Headless::runInteractive( QString fName )
{
	QTextStream stream(stdin);
	QString line;
	do {
		printf("> ");
		line = stream.readLine();
		processLine( line, fName );
	} while (!line.isNull());
}

void Headless::runScript( const QString scriptFileName, QString fName )
{
	if (!QFile::exists( scriptFileName )) {
		printf("Could not open plan file \"%s\"\n", qPrintable(scriptFileName));
		exit(1);
	}
		
	QFile file(scriptFileName);
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
		printf("Could not read plan file \"%s\"\n", qPrintable(scriptFileName));
		exit(1);
	}
		
	QTextStream in(&file);
	while (!in.atEnd()) {
		QString line = in.readLine();
		processLine( line, fName );
	}
}

void Headless::runHeadless( QString headlessCMD, QString fName )
{	
	if( "--headless" == headlessCMD ){
		if( fName != "" ){
			openProject( fName );
		}
		runInteractive( fName );
	} else {
		if( fName != "" ){
			openProject( fName );
		}

		QString planFileName = headlessCMD;
		planFileName.replace("--headless=", "");

		runScript( planFileName, fName );
	}
}

void Headless::setOmniExecutablePath( QString rel_fnpn )
{
	QFileInfo fInfo(rel_fnpn);
	QString fnpn = fInfo.absoluteFilePath();

	OmStateManager::setOmniExecutableAbsolutePath( fnpn );
}

int Headless::start(int argc, char *argv[])
{
	argc_global = argc;
	argv_global = argv;

	CmdLineArgs args = parseAnythingYouCan(argc, argv);
	if ( args.fileArgIndex < 0){
		return 0;
	}

	QString fName = "";
	if ( args.fileArgIndex > 0 ) {
		fName = QString::fromStdString( argv[ args.fileArgIndex ] );
	}

#ifdef Q_WS_X11
	bool useGUI = getenv("DISPLAY") != 0;
#else
	bool useGUI = true;
#endif
	
	setOmniExecutablePath( QString( argv[0] ) );
	if(!useGUI){
		printf("No GUI detected; Running headless....\n");
		runHeadless("--headless", fName);
		return 0;
	} else if( args.runHeadless ){
		runHeadless( args.headlessCMD, fName );
		return 0;
	} else {
		// This is leaking the app, and it is not being freed before app death.
		QApplication * app = new QApplication(argc, argv);
		Q_INIT_RESOURCE(resources);
		MainWindow mainWin;
		mainWin.show();
		
		if ( args.fileArgIndex > 0 ) {
			mainWin.openProject( fName );
		}
		
		return app->exec();
	}
}

void Headless::runMeshPlan( QString headlessLine )
{
	QStringList headlessArgs = headlessLine.split(':');
	QString planName = headlessArgs[1];

	QFile file( planName );
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
		throw OmIoException( "Could not read file" );
	}
	
	QSet<OmId> segmentationIDs;

	QString line;
	QTextStream in(&file);
	while (!in.atEnd()) {
		line = in.readLine();

		OmSegmentationChunkCoord coord = makeChunkCoord( line );
		OmId segmentationID = coord.getSegmentationID();

		if (!segmentationIDs.contains(segmentationID)){
			segmentationIDs << segmentationID;
		}

		OmProject::GetSegmentation( segmentationID ).QueueUpMeshChunk( coord );
	}

	foreach( OmId segID, segmentationIDs){
		OmProject::GetSegmentation( segID ).RunMeshQueue();
	}
}

OmSegmentationChunkCoord Headless::makeChunkCoord( QString line )
{
	QStringList args = line.split(':');
	OmId segmentationID = StringHelpers::getUInt( args[1] );
	unsigned int mipLevel = StringHelpers::getUInt( args[2] );
	QStringList coords = args[3].split(',');
	unsigned int x = StringHelpers::getUInt( coords[0] );
	unsigned int y = StringHelpers::getUInt( coords[1] );
	unsigned int z = StringHelpers::getUInt( coords[2] );

	return OmSegmentationChunkCoord(segmentationID, mipLevel, x, y, z);
}
