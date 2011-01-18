#include "actions/omActions.hpp"
#include "common/omDebug.h"
#include "datalayer/fs/omMemMappedFileQT.hpp"
#include "datalayer/omDataLayer.h"
#include "datalayer/omDataPaths.h"
#include "gui/recentFileList.h"
#include "headless/headless.h"
#include "headless/headlessImpl.hpp"
#include "mesh/omMipMesh.h"
#include "project/omProject.h"
#include "segment/omSegmentCache.h"
#include "system/omLocalPreferences.hpp"
#include "system/omProjectData.h"
#include "system/omStateManager.h"
#include "tiles/omTileDumper.hpp"
#include "utility/dataWrappers.h"
#include "utility/omStringHelpers.h"
#include "viewGroup/omViewGroupState.h"
#include "volume/build/omBuildChannel.hpp"
#include "volume/build/omBuildSegmentation.hpp"
#include "volume/omFilter2d.h"
#include "volume/omMipChunk.h"
#include "volume/omSegmentation.h"
#include "volume/omVolume.h"
#include "zi/base/base.h"

#include <QFileInfo>

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
		HeadlessImpl::OpenProject(fName);
	}

	if("" == headlessCMD){
		runInteractive(fName);
	} else {
		runScript(headlessCMD, fName);
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
		OmActions::Save();

	} else if("mesh" == line) {
		if(0 == segmentationID_ ){
			printf("Please choose segmentation first!\n");
			return;
		}
		const SegmentationDataWrapper sdw(segmentationID_);
		OmBuildSegmentation bs(sdw);
		bs.BuildMesh(om::BLOCKING);

	} else if("clearMST" == line) {
		if(0 == segmentationID_ ){
			printf("Please choose segmentation first!\n");
			return;
		}
		HeadlessImpl::ClearMST(segmentationID_);

	} else if("rebuildCenter" == line) {
		if(0 == segmentationID_ ){
			printf("Please choose segmentation first!\n");
			return;
		}
		HeadlessImpl::RebuildCenterOfSegmentData(segmentationID_);

	} else if("revalidate" == line) {
		if(0 == segmentationID_ ){
			printf("Please choose segmentation first!\n");
			return;
		}
		HeadlessImpl::ReValidateEveryObject(segmentationID_);

	} else if("loadDend" == line) {
		if(0 == segmentationID_ ){
			printf("Please choose segmentation first!\n");
			return;
		}
		const SegmentationDataWrapper sdw(segmentationID_);
		OmBuildSegmentation bs(sdw);
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

		int id1 = OmStringHelpers::getUInt(channelIDs[0]);
		int id2 = OmStringHelpers::getUInt(channelIDs[1]);

		bool verbose = 0;

		if (3 == args.size()){
			verbose = (bool) OmStringHelpers::getUInt(args[2]);
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

		const int id1 = OmStringHelpers::getUInt(segmentationIDs[0]);
		const int id2 = OmStringHelpers::getUInt(segmentationIDs[1]);

		bool verbose = 0;

		if (3 == args.size()){
			verbose = (bool) OmStringHelpers::getUInt(args[2]);
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
		OmID segID = 1;

		if(3 == args.size()) {
			segID = OmStringHelpers::getUInt(args[1]);
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
		OmID segID = 1;

		if(3 == args.size()) {
			segID = OmStringHelpers::getUInt(args[1]);
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
		segmentationID_ = OmStringHelpers::getUInt(args[1]);
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
		HeadlessImpl::OpenProject(args[1]);

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
		OmActions::Save();

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

		HeadlessImpl::loadHDF5seg(args[1], segmentationID_);

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
		bc.BuildBlocking();

        } else if(line.startsWith("loadHDF5affgraph:")){
                QStringList args = line.split(':',QString::SkipEmptyParts);

                if (args.size() < 3){
                        printf("Please enter command like: loadHDF5affgraph:[0123]:file.hdf5\n");
                        return;
                }
		om::Affinity aff = om::NO_AFFINITY;
		int affNum = args[1].toInt();
		if(1 == affNum) {
			aff = om::X_AFFINITY;
		} else if(2 == affNum) {
			aff = om::Y_AFFINITY;
		} else if(3 == affNum) {
			aff = om::Z_AFFINITY;
		}
		printf("args: %s, %i, %i\n", qPrintable(args[1]), affNum, aff);
                OmChannel& affgraph = OmProject::AddChannel(aff);
                QString hdf5fnp = args[2];

                OmBuildChannel bc(&affgraph);
                bc.addFileNameAndPath(hdf5fnp);
                bc.BuildBlocking();

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
		bc.BuildBlocking();

	} else if(line.startsWith("loadTIFFseg:")){
		QStringList args = line.split(':',QString::SkipEmptyParts);

		if (args.size() < 2){
			printf("Please enter a path to a directory.\n");
			return;
		}

		OmBuildSegmentation bs;
		segmentationID_ = bs.GetDataWrapper().getID();

		QDir dir(args[1]);
		foreach(QFileInfo f, dir.entryInfoList()){
			if(!f.isFile()){
				continue;
			}
			//printf("adding %s/\n", qPrintable(f.canonicalFilePath()));
			bs.addFileNameAndPath(f.canonicalFilePath());
		}

		bs.BuildImage(om::BLOCKING);

	} else if(line.startsWith("buildHDF5:")){
		QStringList args = line.split(':',QString::SkipEmptyParts);

		if (args.size() < 2){
			printf("Please enter a filename.\n");
			return;
		}

		HeadlessImpl::buildHDF5(args[1]);

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

		int channID = OmStringHelpers::getUInt(args[1]);

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

		int segID = OmStringHelpers::getUInt(args[1]);

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

	} else if(line.startsWith("setChanResolution:")){
		const QStringList args = line.split(':',QString::SkipEmptyParts);

		if (args.size() != 2){
			printf("format is setChannResolution:channID,xRes,yRes,zRes\n");
			return;
		}

		const QStringList res = args[1].split(',', QString::SkipEmptyParts);

		if (res.size() != 4){
			printf("format is setChannResolution:channID,xRes,yRes,zRes\n");
			return;
		}

		const OmID channID = OmStringHelpers::getUInt(res[0]);
		const float xRes = OmStringHelpers::getFloat(res[1]);
		const float yRes = OmStringHelpers::getFloat(res[2]);
		const float zRes = OmStringHelpers::getFloat(res[3]);

		ChannelDataWrapper cdw(channID);

		if (!cdw.IsChannelValid()){
			printf("Channel %i is not a valid channel.\n", channID);
			return;
		}

		HeadlessImpl::ChangeVolResolution(cdw.GetChannel(), xRes, yRes, zRes);

	} else if(line.startsWith("setSegResolution:")){
		const QStringList args = line.split(':',QString::SkipEmptyParts);

		if (args.size() != 2){
			printf("format is setSegResolution:channID,xRes,yRes,zRes\n");
			return;
		}

		const QStringList res = args[1].split(',', QString::SkipEmptyParts);

		if (res.size() != 4){
			printf("format is setSegResolution:segID,xRes,yRes,zRes\n");
			return;
		}

		const OmID segID = OmStringHelpers::getUInt(res[0]);
		const float xRes = OmStringHelpers::getFloat(res[1]);
		const float yRes = OmStringHelpers::getFloat(res[2]);
		const float zRes = OmStringHelpers::getFloat(res[3]);

		SegmentationDataWrapper sdw(segID);

		if (!sdw.IsSegmentationValid()){
			printf("Segmentation %i is not a valid segmentation\n", segID);
			return;
		}

		HeadlessImpl::ChangeVolResolution(sdw.GetSegmentation(), xRes, yRes, zRes);

	} else if(line.startsWith("setMeshDownScallingFactor:")){
		const QStringList args = line.split(':',QString::SkipEmptyParts);

		if (args.size() != 2){
			printf("format is setMeshDownScallingFactor:factor (factor is double)\n");
			return;
		}

		const double factor = OmStringHelpers::getDouble(args[1]);

		HeadlessImpl::SetMeshDownScallingFactor(factor);

	} else if(line.startsWith("dumpSegmentColors")){
		if(0 == segmentationID_ ){
			printf("Please choose segmentation first!\n");
			return;
		}
		HeadlessImpl::DumpSegmentColorHistograms(segmentationID_);

	} else if(line.startsWith("dumpRootSegmentColors")){
		if(0 == segmentationID_ ){
			printf("Please choose segmentation first!\n");
			return;
		}
		HeadlessImpl::DumpRootSegmentColorHistograms(segmentationID_);

	} else if(line.startsWith("recolorSegments")){
		if(0 == segmentationID_ ){
			printf("Please choose segmentation first!\n");
			return;
		}
		HeadlessImpl::RecolorAllSegments(segmentationID_);

	} else if(line.startsWith("importWatershed:")){
		const QStringList args = line.split(':',QString::SkipEmptyParts);

		if (args.size() != 2){
			printf("format is importWatershed:metadataFileName\n");
			return;
		}

		HeadlessImpl::ImportWatershed(args[1]);

	} else {
		printf("Could not parse \"%s\".\n", qPrintable(line));
	}
}

