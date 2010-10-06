#include "mesh/omMipMesh.h"
#include "common/omDebug.h"
#include "datalayer/fs/omMemMappedFileQT.hpp"
#include "datalayer/omDataLayer.h"
#include "datalayer/omDataPaths.h"
#include "headless/headless.h"
#include "gui/recentFileList.h"
#include "project/omProject.h"
#include "project/omProjectSaveAction.h"
#include "segment/omSegmentCache.h"
#include "system/omBuildChannel.h"
#include "system/omBuildSegmentation.h"
#include "system/omLocalPreferences.h"
#include "system/omProjectData.h"
#include "system/omStateManager.h"
#include "viewGroup/omViewGroupState.h"
#include "tiles/omTileDumper.hpp"
#include "utility/dataWrappers.h"
#include "utility/stringHelpers.h"
#include "volume/omFilter2d.h"
#include "volume/omMipChunk.h"
#include "volume/omSegmentation.h"
#include "volume/omVolume.h"
#include "zi/base/base.h"
#include "zi/watershed/RawQuickieWS.h"

#include <QFileInfo>

void Headless::openProject(const QString& fName)
{
	try {
		printf("Please wait: Opening project \"%s\"...\n", qPrintable(fName));
		OmProject::Load(fName);
		//Set current working directory to file path
		QDir::setCurrent(QFileInfo(fName).absolutePath());
		printf("Opened project \"%s\"\n", qPrintable(fName));
	} catch(...) {
		printf("Error while loading project \"%s\"\n", qPrintable(fName));
	}
}

void Headless::processLine(const QString& line, const QString&)
{
	OmTimer timer;

	if(line == "q" || line == "quit"){
		printf("Exiting...\n");
		OmProject::Close();
		exit(0);
	} else if("save" == line) {
		(new OmProjectSaveAction())->Run();
	} else if("mesh" == line) {
		if(0 == segmentationID_ ){
			printf("Please choose segmentation first!\n");
			return;
		}
		OmSegmentation& added_segmentation = OmProject::GetSegmentation(segmentationID_);
		OmBuildSegmentation bs(&added_segmentation);
		bs.build_seg_mesh();
		bs.wait();
	} else if("loadDend" == line) {
		if(0 == segmentationID_ ){
			printf("Please choose segmentation first!\n");
			return;
		}
		OmSegmentation& added_segmentation = OmProject::GetSegmentation(segmentationID_);
		OmBuildSegmentation bs(&added_segmentation);
		bs.loadDendrogram();
	} else if(line.startsWith("compareChanns:")) {
		// format: compareChanns:id1,id2[:verbose]
		QStringList args = line.split(':',QString::SkipEmptyParts);

		if (args.size() < 2){
			printf("Please specify channel IDs.\n");
			return;
		}

		QStringList channelIDs = args[1].split(',',QString::SkipEmptyParts);

		if (channelIDs.size() < 2){
			printf("Not enough channel IDs specified. Please specify as 'id1,id2'.\n");
			return;
		}

		int id1 = StringHelpers::getUInt(channelIDs[0]);
		int id2 = StringHelpers::getUInt(channelIDs[1]);

		bool verbose = 0;

		if (3 == args.size()){
			verbose = (bool) StringHelpers::getUInt(args[2]);
		}

		if(!OmProject::IsChannelValid(id1)) {
			printf("Channel %i is not a valid channel.\n",id1);
			return;
		}
		if(!OmProject::IsChannelValid(id2)) {
			printf("Channel %i is not a valid channel.\n",id2);
			return;
		}
		const bool same =
			OmMipVolume::CompareVolumes(&OmProject::GetChannel(id1),
										&OmProject::GetChannel(id2));

		if(same){
			printf("Channel %i and Channel %i are identical.\n",id1,id2);
		} else {
			printf("Channel %i and Channel %i are different!\n",id1,id2);
		}
	} else if(line.startsWith("compareSegs:")) {
		// format: compareSegs:id1,id2[:verbose]
		QStringList args = line.split(':',QString::SkipEmptyParts);

		if (args.size() < 2){
			printf("Please specify segmentation IDs.\n");
			return;
		}

		QStringList segmentationIDs = args[1].split(',',QString::SkipEmptyParts);

		if (segmentationIDs.size() < 2){
			printf("Not enough segmentation IDs specified. Please specify as 'id1,id2'.\n");
			return;
		}

		const int id1 = StringHelpers::getUInt(segmentationIDs[0]);
		const int id2 = StringHelpers::getUInt(segmentationIDs[1]);

		bool verbose = 0;

		if (3 == args.size()){
			verbose = (bool) StringHelpers::getUInt(args[2]);
		}

		if(!OmProject::IsSegmentationValid(id1)) {
			printf("Segmentation %i is not a valid segmentation.\n",id1);
			return;
		}

		if(!OmProject::IsSegmentationValid(id2)) {
			printf("Segmentation %i is not a valid segmentation.\n",id2);
			return;
		}
		const bool same =
			OmMipVolume::CompareVolumes(&OmProject::GetSegmentation(id1),
										&OmProject::GetSegmentation(id2));
		if(same){
			printf("Segmentation %i and Segmentation %i are identical.\n",id1,id2);
		} else {
			printf("Segmentation %i and Segmentation %i are different!\n",id1,id2);
		}
	} else if(line.startsWith("dumpSegTiles:")) {
		QStringList args = line.split(':',QString::SkipEmptyParts);
		QString fileNameAndPath = "/tmp/tiles.dump";
		OmId segID = 1;

		if(3 == args.size()) {
			segID = StringHelpers::getUInt(args[1]);
			fileNameAndPath = args[2];
		} else if(2 == args.size()) {
			segID = 1;
			fileNameAndPath = args[1];
		}

		OmViewGroupState* vgs = new OmViewGroupState(NULL);
		OmTileDumper dumper(segID, SEGMENTATION, fileNameAndPath, vgs);
		dumper.DumpTiles();

	} else if(line.startsWith("dumpChannTiles:")) {
		QStringList args = line.split(':',QString::SkipEmptyParts);
		QString file = "/tmp/tiles";
		OmId segID = 1;

		if(3 == args.size()) {
			segID = StringHelpers::getUInt(args[1]);
			file = args[2];
		} else if(2 == args.size()) {
			segID = 1;
			file = args[1];
		}

		OmViewGroupState * vgs = new OmViewGroupState(NULL);
		OmTileDumper dumper(segID, CHANNEL, file, vgs);
		dumper.DumpTiles();
	} else if(line.startsWith("seg:")) {
		QStringList args = line.split(':',QString::SkipEmptyParts);
		if (args.size() < 2){
			printf("Please enter a filename.\n");
			return;
		}
		segmentationID_ = StringHelpers::getUInt(args[1]);
		if(segmentationID_ > 0){
			printf("segmentationID_ set to %d\n", segmentationID_);
		} else {
			printf("Invalid segmentation\n");
		}
	} else if(line.startsWith("open:")){
		QStringList args = line.split(':',QString::SkipEmptyParts);
		if (args.size() < 2){
			printf("Please enter a filename.\n");
			return;
		}
		openProject(args[1]);
	} else if("close" == line){
		OmProject::Close();
		printf("Project closed.\n");
	} else if("pwd" == line){
		printf("%s\n", qPrintable(QDir::currentPath()));
	} else if(line.startsWith("cd ")){
		QStringList args = line.split("cd ", QString::SkipEmptyParts);
		if (args.size() < 1){
			printf("Please enter a pathname.\n");
			return;
		}
		QString path = args[0];
		if (!QDir::QDir(path).exists()){
			printf("Directory does not exist.\n");
			return;
		}
		QDir::setCurrent(path);
	} else if("ls" == line){
		QStringList entrylist = QDir::current().entryList();
		foreach (QString str, entrylist){
			printf("%s\n",qPrintable(str));
		}
	} else if("ls -l" == line){
		QFileInfoList entrylist = QDir::current().entryInfoList();
		foreach(QFileInfo f, entrylist){
			QString line = QString("%1%2")
				.arg(f.fileName(), -20, ' ')
				.arg(QString::number((double)f.size()/BYTES_PER_MB, 'f', 3));
			printf("%s\n", qPrintable(line));
		}
	} else if(line.startsWith("addSegment")){
		if(0 == segmentationID_ ){
			printf("Please choose segmentation first!\n");
			return;
		}
		OmProject::GetSegmentation(segmentationID_).GetSegmentCache()->AddSegment();
		(new OmProjectSaveAction())->Run();
	} else if(line.startsWith("create:")) {
		QStringList args = line.split(':',QString::SkipEmptyParts);

		if (args.size() < 2){
			printf("Please enter a filename.\n");
			return;
		}

		QString projectFileNameAndPath = args[1];
		const QString fname = OmProject::New(projectFileNameAndPath);
		RecentFileList::prependFileToFS(fname);
		printf("Created and opened %s.\n",qPrintable(fname));

	} else if(line.startsWith("createOrOpen:")) {
		QStringList args = line.split(':',QString::SkipEmptyParts);

		if (args.size() < 2){
			printf("Please enter a filename.\n");
			return;
		}

		QString projectFileNameAndPath = args[1];

		QFile file(projectFileNameAndPath);
		if(file.exists()){
			OmProject::Load(projectFileNameAndPath);

		} else {
			OmProject::New(projectFileNameAndPath);
		}

	} else if(line.startsWith("loadHDF5seg:")){
		QStringList args = line.split(':',QString::SkipEmptyParts);

		if (args.size() < 2){
			printf("Please enter a filename.\n");
			return;
		}

		OmSegmentation& added_segmentation = OmProject::AddSegmentation();
		segmentationID_ = added_segmentation.GetId();
		QString hdf5fnp = args[1];

		OmBuildSegmentation bs(&added_segmentation);
		bs.addFileNameAndPath(hdf5fnp);
		bs.build_seg_image();
		bs.wait();
	} else if(line.startsWith("loadHDF5chann:")){
		QStringList args = line.split(':',QString::SkipEmptyParts);

		if (args.size() < 2){
			printf("Please enter a filename.\n");
			return;
		}

		OmChannel& chann = OmProject::AddChannel();
		QString hdf5fnp = args[1];

		OmBuildChannel bc(&chann);
		bc.addFileNameAndPath(hdf5fnp);
		bc.build_channel();
		bc.wait();
	} else if(line.startsWith("loadTIFFchann:")){
		QStringList args = line.split(':',QString::SkipEmptyParts);

		if (args.size() < 2){
			printf("Please enter a path to a directory.\n");
			return;
		}

		OmChannel& chann = OmProject::AddChannel();
		OmBuildChannel bc(&chann);

		QDir dir(args[1]);
		foreach(QFileInfo f, dir.entryInfoList()){
			if(!f.isFile()){
				continue;
			}
			//printf("adding %s/\n", qPrintable(f.canonicalFilePath()));
			bc.addFileNameAndPath(f.canonicalFilePath());
		}
		bc.build_channel();
		bc.wait();
	} else if(line.startsWith("loadTIFFseg:")){
		QStringList args = line.split(':',QString::SkipEmptyParts);

		if (args.size() < 2){
			printf("Please enter a path to a directory.\n");
			return;
		}

		OmSegmentation& seg = OmProject::AddSegmentation();
		segmentationID_ = seg.GetId();
		OmBuildSegmentation bs(&seg);

		QDir dir(args[1]);
		foreach(QFileInfo f, dir.entryInfoList()){
			if(!f.isFile()){
				continue;
			}
			//printf("adding %s/\n", qPrintable(f.canonicalFilePath()));
			bs.addFileNameAndPath(f.canonicalFilePath());
		}
		bs.build_seg_image();
		bs.wait();
	} else if(line.startsWith("buildHDF5:")){
		QStringList args = line.split(':',QString::SkipEmptyParts);

		if (args.size() < 2){
			printf("Please enter a filename.\n");
			return;
		}

		QString projectFileName = QFileInfo(args[1]+".omni").fileName();

		OmProject::New(projectFileName);

		OmSegmentation& added_segmentation = OmProject::AddSegmentation();
		QString hdf5fnp = args[1];

		OmBuildSegmentation bs(&added_segmentation);
		bs.addFileNameAndPath(hdf5fnp);
		bs.buildAndMeshSegmentation();
		bs.wait();
	} else if("loadChunk" == line){
		if(0 == segmentationID_){
			printf("Please choose segmentation first!\n");
			return;
		}

		OmSegmentation& segmen = OmProject::GetSegmentation(segmentationID_);
		OmMipChunkCoord chunk_coord(0,0,0,0);
		OmMipChunkPtr p_chunk;
		segmen.GetChunk(p_chunk, chunk_coord);
	} else if(line.startsWith("removeChann:")){
		QStringList args = line.split(':',QString::SkipEmptyParts);

		if (args.size() < 2){
			printf("Please enter a channel id.\n");
			return;
		}

		int channID = StringHelpers::getUInt(args[1]);

		if (!OmProject::IsChannelValid(channID)){
			printf("Channel %i is not a valid channel.\n",channID);
			return;
		}

		OmProject::RemoveChannel(channID);
		printf("Channel %i removed.\n",channID);

	} else if(line.startsWith("removeSeg:")){
		QStringList args = line.split(':',QString::SkipEmptyParts);

		if (args.size() < 2){
			printf("Please enter a segmentation id.\n");
			return;
		}

		int segID = StringHelpers::getUInt(args[1]);

		if (!OmProject::IsSegmentationValid(segID)){
			printf("Segmentation %i is not a valid segmentation.\n",segID);
			return;
		}

		OmProject::RemoveSegmentation(segID);
		printf("Segmentation %i removed.\n",segID);

	} else if("lsChann" == line){
		OmIDsSet channset = OmProject::GetValidChannelIds();
		if (channset.empty()){
			printf("No channels present.\n");
			return;
		}
		printf("ID\tName\n");
		FOR_EACH(iter,channset){
			ChannelDataWrapper cdw(*iter);
			printf("%i\t%s\n", *iter, qPrintable(cdw.getName()));
		}
	} else if("lsSeg" == line){
		OmIDsSet segset = OmProject::GetValidSegmentationIds();
		if (segset.empty()){
			printf("No segmentations present.\n");
			return;
		}
		printf("ID\tName\n");
		FOR_EACH(iter,segset){
			SegmentationDataWrapper sdw(*iter);
			printf("%i\t%s\n", *iter, qPrintable(sdw.getName()));
		}
	} else if(line.startsWith("watershed:")){
		watershed(line);
	} else {
		printf("Could not parse \"%s\".\n", qPrintable(line));
	}
}

void Headless::runInteractive(const QString& fName)
{
	QTextStream stream(stdin);
	QString line;
	do {
		printf("> ");
		line = stream.readLine();
		processLine(line, fName);
	} while (!line.isNull());
}

void Headless::runScript(const QString scriptFileName, const QString& fName)
{
	if (!QFile::exists(scriptFileName)) {
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
		processLine(line, fName);
	}
}

void Headless::runHeadless(const QString& headlessCMD, const QString& fName)
{
	if(fName != ""){
		openProject(fName);
	}

	if("" == headlessCMD){
		runInteractive(fName);
	} else {
		runScript(headlessCMD, fName);
	}
}

void Headless::watershed(const QString&)
{
/*
	const QStringList argsMain = line.split(':',QString::SkipEmptyParts);

	if (argsMain.size() < 2){
		printf("Please enter an input filename, x, y, and z, low, high, size, and absLowThreshold.\n");
		return;
	}

	const QStringList args = argsMain[1].split(',',QString::SkipEmptyParts);
	if (args.size() < 8){
		printf("%d args; ", args.size());
		printf("Please enter an input filename, x, y, and z, low, high, size, and absLowThreshold.\n");
		return;
	}

	const int64_t xDim = StringHelpers::getUInt(args[1]);
	const int64_t yDim = StringHelpers::getUInt(args[2]);
	const int64_t zDim = StringHelpers::getUInt(args[3]);
	const size_t numVoxels = xDim*yDim*zDim;
	const size_t numBytesIn = numVoxels*sizeof(float)*3;
	const size_t numBytesOut = numVoxels*sizeof(int);

	const std::string in_fnp = args[0].toStdString();
	OmMemMappedFileReadQT<float> in(in_fnp, numBytesIn);

	const std::string out_fnp = in_fnp + ".out";
	OmMemMappedFileWriteQT<int> out(out_fnp, numBytesOut);

	const float loThreshold = StringHelpers::getFloat(args[4]);
	const float hiThreshold = StringHelpers::getFloat(args[5]);
	const int numThreshold = StringHelpers::getUInt(args[6]);
	const float absLowThreshold = StringHelpers::getFloat(args[7]);

	RawQuickieWS rqws(xDim,
					  yDim,
					  zDim,
					  loThreshold,
					  hiThreshold,
					  numThreshold,
					  absLowThreshold);

	rqws.Run(in.GetPtr(), out.GetPtr());

	const size_t numEdges = rqws.GetNumMSTedges();
	const qint64 numBytesMST = numEdges*sizeof(OmMSTedge);

	const std::string mst_fnp = out_fnp + ".mst";
	OmMemMappedFileWriteQT<OmMSTedge> mst(mst_fnp, numBytesMST);

	rqws.SaveToMemMap(mst.GetPtr());
*/
}
