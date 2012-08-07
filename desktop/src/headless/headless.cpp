#include "actions/omActions.h"
#include "chunks/omChunk.h"
#include "common/omDebug.h"
#include "datalayer/fs/omMemMappedFileQT.hpp"
#include "datalayer/omDataPaths.h"
#include "gui/recentFileList.h"
#include "headless/headless.h"
#include "headless/headlessImpl.hpp"
#include "mesh/omMesh.h"
#include "segment/omSegments.h"
#include "system/omLocalPreferences.hpp"
#include "system/omStateManager.h"
#include "tiles/omTileDumper.hpp"
#include "utility/dataWrappers.h"
#include "utility/omStringHelpers.h"
#include "viewGroup/omViewGroupState.h"
#include "volume/build/omBuildAffinityChannel.hpp"
#include "volume/build/omBuildChannel.hpp"
#include "volume/build/omBuildSegmentation.hpp"
#include "volume/build/omSegmentationRebuilder.hpp"
#include "volume/build/omVolumeBuilder.hpp"
#include "volume/omAffinityGraph.h"
#include "volume/omCompareVolumes.hpp"
#include "volume/omFilter2d.h"
#include "volume/omSegmentation.h"

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

void Headless::RunHeadless(const QString& headlessCMD, const QString& fName)
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

        HeadlessImpl::Mesh(segmentationID_);

    } else if("DumpActions" == line) {
        HeadlessImpl::DumpActionsToTextFile();

    } else if(line.startsWith("mesh:")) {
        if(0 == segmentationID_ ){
            printf("Please choose segmentation first!\n");
            return;
        }

        QStringList args = line.split(':',QString::SkipEmptyParts);

        if (args.size() < 2){
            printf("Please specify threshold\n");
            return;
        }

        // const double threshold = OmStringHelpers::getDouble(args[1]);

        const SegmentationDataWrapper sdw(segmentationID_);
        OmBuildSegmentation bs(sdw);
        // bs.BuildMesh(threshold);
        assert(0 && "threshold option disabled");

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
        bs.LoadDendrogram();

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

        if(!ChannelDataWrapper(id1).IsChannelValid()){
            printf("Channel %i is not a valid channel.\n",id1);
            return;
        }
        if(!ChannelDataWrapper(id2).IsChannelValid()){
            printf("Channel %i is not a valid channel.\n",id2);
            return;
        }

        const bool same = OmCompareVolumes::CompareChannels(id1, id2);

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

        if(!SegmentationDataWrapper(id1).IsSegmentationValid()){
            printf("Segmentation %i is not a valid segmentation.\n",id1);
            return;
        }
        if(!SegmentationDataWrapper(id2).IsSegmentationValid()){
            printf("Segmentation %i is not a valid segmentation.\n",id2);
            return;
        }

        const bool same = OmCompareVolumes::CompareSegmentations(id1, id2);

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

        SegmentationDataWrapper sdw(segID);

        OmViewGroupState* vgs = new OmViewGroupState(NULL);
        OmTileDumper dumper(sdw.GetSegmentationPtr(), fileNameAndPath, vgs);
        dumper.DumpTiles();

    } else if(line.startsWith("dumpChannTiles:")) {
        QStringList args = line.split(':',QString::SkipEmptyParts);
        QString file = "/tmp/tiles";
        OmID chanID = 1;

        if(3 == args.size()) {
            chanID = OmStringHelpers::getUInt(args[1]);
            file = args[2];
        } else if(2 == args.size()) {
            chanID = 1;
            file = args[1];
        }

        ChannelDataWrapper cdw(chanID);

        OmViewGroupState * vgs = new OmViewGroupState(NULL);
        OmTileDumper dumper(cdw.GetChannelPtr(), file, vgs);
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
        if (!QDir(path).exists()){
            printf("Directory does not exist.\n");
            return;
        }
        QDir::setCurrent(path);

    } else if("ls" == line){
        QStringList entrylist = QDir::current().entryList();
        Q_FOREACH (QString str, entrylist){
            printf("%s\n",qPrintable(str));
        }

    } else if("ls -l" == line){
        QFileInfoList entrylist = QDir::current().entryInfoList();
        Q_FOREACH(QFileInfo f, entrylist){
            QString line = QString("%1%2")
                .arg(f.fileName(), -20, ' ')
                .arg(QString::number((double)f.size()/om::math::bytesPerMB, 'f', 3));
            printf("%s\n", qPrintable(line));
        }

    } else if(line.startsWith("addSegment")){
        if(0 == segmentationID_ ){
            printf("Please choose segmentation first!\n");
            return;
        }
        SegmentationDataWrapper sdw(segmentationID_);
        sdw.Segments()->AddSegment();
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

        ChannelDataWrapper cdw;
        OmChannel& chann = cdw.Create();
        QString hdf5fnp = args[1];

        OmBuildChannel bc(&chann);
        bc.addFileNameAndPath(hdf5fnp);
        bc.Build();

    } else if(line.startsWith("loadHDF5affgraph:")){
        QStringList args = line.split(':',QString::SkipEmptyParts);

        if (args.size() < 2){
            printf("Please enter command like: loadHDF5affgraph:file.hdf5\n");
            return;
        }

        const QString& hdf5fnp = args[1];

        AffinityGraphDataWrapper agdw;
        OmAffinityGraph& affGraph = agdw.Create();
        affGraph.ImportAllChannels(hdf5fnp);

    } else if(line.startsWith("loadTIFFchann:")){
        QStringList args = line.split(':',QString::SkipEmptyParts);

        if (args.size() < 2){
            printf("Please enter a path to a directory.\n");
            return;
        }

        ChannelDataWrapper cdw;
        OmChannel& chann = cdw.Create();
        OmBuildChannel bc(&chann);

        QDir dir(args[1]);
        Q_FOREACH(QFileInfo f, dir.entryInfoList())
        {
            if(!f.isFile()){
                continue;
            }
            //printf("adding %s/\n", qPrintable(f.canonicalFilePath()));
            bc.addFileNameAndPath(f.canonicalFilePath());
        }
        bc.Build();

    } else if(line.startsWith("loadTIFFseg:")){
        QStringList args = line.split(':',QString::SkipEmptyParts);

        if (args.size() < 2){
            printf("Please enter a path to a directory.\n");
            return;
        }

        OmBuildSegmentation bs;
        segmentationID_ = bs.GetDataWrapper().GetID();

        QDir dir(args[1]);

        Q_FOREACH(QFileInfo f, dir.entryInfoList())
        {
            if(!f.isFile()){
                continue;
            }
            //printf("adding %s/\n", qPrintable(f.canonicalFilePath()));
            bs.addFileNameAndPath(f.canonicalFilePath());
        }

        bs.BuildImage();

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

        SegmentationDataWrapper sdw(segmentationID_);
        OmSegmentation& segmen = sdw.GetSegmentation();
        om::chunkCoord chunk_coord(0,0,0,0);
        OmChunk* chunk = segmen.GetChunk(chunk_coord);
        assert(chunk);

    } else if(line.startsWith("removeChann:")){
        QStringList args = line.split(':',QString::SkipEmptyParts);

        if (args.size() < 2){
            printf("Please enter a channel id.\n");
            return;
        }

        int channID = OmStringHelpers::getUInt(args[1]);

        if (!ChannelDataWrapper(channID).IsChannelValid()){
            printf("Channel %i is not a valid channel.\n",channID);
            return;
        }

        ChannelDataWrapper::Remove(channID);
        printf("Channel %i removed.\n",channID);

    } else if(line.startsWith("removeSeg:")){
        QStringList args = line.split(':',QString::SkipEmptyParts);

        if (args.size() < 2){
            printf("Please enter a segmentation id.\n");
            return;
        }

        int segID = OmStringHelpers::getUInt(args[1]);

        if (!SegmentationDataWrapper(segID).IsSegmentationValid()){
            printf("Segmentation %i is not a valid segmentation.\n",segID);
            return;
        }

        SegmentationDataWrapper::Remove(segID);
        printf("Segmentation %i removed.\n",segID);

    } else if("lsChann" == line){
        const OmIDsSet& channset = ChannelDataWrapper::ValidIDs();

        if (channset.empty()){
            printf("No channels present.\n");
            return;
        }

        printf("ID\tName\n");

        FOR_EACH(iter, channset)
        {
            ChannelDataWrapper cdw(*iter);
            printf("%i\t%s\n", *iter, qPrintable(cdw.GetName()));
        }

    } else if("lsSeg" == line){
        const OmIDsSet& segset = SegmentationDataWrapper::ValidIDs();

        if (segset.empty()){
            printf("No segmentations present.\n");
            return;
        }

        printf("ID\tName\n");

        FOR_EACH(iter,segset)
        {
            SegmentationDataWrapper sdw(*iter);
            printf("%i\t%s\n", *iter, qPrintable(sdw.GetName()));
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

    } else if(line.startsWith("setChanAbsOffset:")){
        const QStringList args = line.split(':',QString::SkipEmptyParts);

        if (args.size() != 2)
        {
            printf("format is setChanAbsOffset:channID,xRes,yRes,zRes\n");
            return;
        }

        const QStringList res = args[1].split(',', QString::SkipEmptyParts);

        if (res.size() != 4)
        {
            printf("format is setChanAbsOffset:segID,xRes,yRes,zRes\n");
            return;
        }

        const OmID segID = OmStringHelpers::getUInt(res[0]);
        const int xRes = OmStringHelpers::getUInt(res[1]);
        const int yRes = OmStringHelpers::getUInt(res[2]);
        const int zRes = OmStringHelpers::getUInt(res[3]);

        ChannelDataWrapper cdw(segID);

        if (!cdw.IsValidWrapper())
        {
            printf("Channel %i is not a valid channel\n", segID);
            return;
        }

        HeadlessImpl::ChangeVolAbsOffset(cdw.GetChannel(), xRes, yRes, zRes);

    } else if(line.startsWith("setSegAbsOffset:")){
        const QStringList args = line.split(':',QString::SkipEmptyParts);

        if (args.size() != 2)
        {
            printf("format is setSegAbsOffset:channID,xRes,yRes,zRes\n");
            return;
        }

        const QStringList res = args[1].split(',', QString::SkipEmptyParts);

        if (res.size() != 4)
        {
            printf("format is setSegAbsOffset:segID,xRes,yRes,zRes\n");
            return;
        }

        const OmID segID = OmStringHelpers::getUInt(res[0]);
        const int xRes = OmStringHelpers::getUInt(res[1]);
        const int yRes = OmStringHelpers::getUInt(res[2]);
        const int zRes = OmStringHelpers::getUInt(res[3]);

        SegmentationDataWrapper sdw(segID);

        if(!sdw.IsValidWrapper())
        {
            printf("Segmentation %i is not a valid segmentation\n", segID);
            return;
        }

        HeadlessImpl::ChangeVolAbsOffset(sdw.GetSegmentation(), xRes, yRes, zRes);

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

    } else if("refindUniqueChunkValues" == line){
        if(0 == segmentationID_ ){
            printf("Please choose segmentation first!\n");
            return;
        }
        OmChunkUtils::RefindUniqueChunkValues(segmentationID_);


    } else if(line.startsWith("importWatershed:")){
        const QStringList args = line.split(':',QString::SkipEmptyParts);

        if (args.size() != 2){
            printf("format is importWatershed:metadataFileName\n");
            return;
        }

        HeadlessImpl::ImportWatershed(args[1]);

    } else if(line.startsWith("timeSegChunkReads:")){
        const QStringList args = line.split(':',QString::SkipEmptyParts);

        if (args.size() != 4){
            printf("format is timeSegChunkReads:segmentationID:randomize:useFseek\n");
            return;
        }

        const OmID segID = OmStringHelpers::getUInt(args[1]);
        const bool randomizeOrder = OmStringHelpers::getBool(args[2]);
        const bool useRawChunk = OmStringHelpers::getBool(args[3]);

        HeadlessImpl::TimeSegChunkReads(segID, randomizeOrder, useRawChunk);

    } else if(line.startsWith("downsampleChan:")){
        QStringList args = line.split(':',QString::SkipEmptyParts);

        if (args.size() < 2)
        {
            printf("Please enter a channel id.\n");
            return;
        }

        int channID = OmStringHelpers::getUInt(args[1]);

        if (!ChannelDataWrapper(channID).IsChannelValid())
        {
            printf("Channel %i is not a valid channel.\n",channID);
            return;
        }

        ChannelDataWrapper cdw(channID);
        if(cdw.IsValidWrapper()){
            HeadlessImpl::DownsampleChannel(cdw);
        }

    } else if(line.startsWith("checkMeshes:")){
        QStringList args = line.split(':',QString::SkipEmptyParts);

        if (args.size() < 2)
        {
            printf("Please enter a segmentation id.\n");
            return;
        }

        const OmID segID = OmStringHelpers::getUInt(args[1]);

        SegmentationDataWrapper sdw(segID);

        if (!sdw.IsValidWrapper())
        {
            printf("segmentation id %i is not a valid segmentation.\n", segID);
            return;
        }

        HeadlessImpl::CheckMeshes(sdw);

    } else if (line.startsWith("replaceChanSlice:")){

        QStringList args = line.split(':',QString::SkipEmptyParts);

        if (args.size() < 4)
        {
            printf("Please enter as replaceChanSlice::channelID::fileName::sliceNum.\n");
            return;
        }

        const OmID channelID = OmStringHelpers::getUInt(args[1]);

        ChannelDataWrapper cdw(channelID);

        if (!cdw.IsValidWrapper())
        {
            printf("channel id %i is not a valid channel.\n", channelID);
            return;
        }

        const QString fname = args[2];
        const int sliceNum = OmStringHelpers::getUInt(args[3]);

        HeadlessImpl::ReplaceSlice(cdw.GetChannelPtr(), fname, sliceNum);

   } else if (line.startsWith("replaceSegSlice:")){

        QStringList args = line.split(':',QString::SkipEmptyParts);

        if (args.size() < 4)
        {
            printf("Please enter as replaceSegSlice::segmentationID::fileName::sliceNum.\n");
            return;
        }

        const OmID segmentationID = OmStringHelpers::getUInt(args[1]);

        SegmentationDataWrapper sdw(segmentationID);

        if(!sdw.IsValidWrapper())
        {
            printf("segmentation id %i is not a valid segmentation.\n", segmentationID);
            return;
        }

        const QString fname = args[2];
        const int sliceNum = OmStringHelpers::getUInt(args[3]);

        HeadlessImpl::ReplaceSlice(sdw.GetSegmentationPtr(), fname, sliceNum);

    } else if(line.startsWith("exportSegRaw:")){
        QStringList args = line.split(':',QString::SkipEmptyParts);

        if (args.size() < 3)
        {
            printf("Please enter a segmentation id and export file name.\n");
            return;
        }

        const OmID segID = OmStringHelpers::getUInt(args[1]);

        SegmentationDataWrapper sdw(segID);

        if (!sdw.IsValidWrapper())
        {
            printf("segmentation id %i is not a valid segmentation.\n", segID);
            return;
        }

        HeadlessImpl::ExportSegmentationRaw(sdw, args[2]);

    } else if(line.startsWith("rebuildSegmentation:")){
        QStringList args = line.split(':',QString::SkipEmptyParts);

        if (args.size() < 2)
        {
            printf("Please enter a segmentation id.\n");
            return;
        }

        const OmID segID = OmStringHelpers::getUInt(args[1]);

        SegmentationDataWrapper sdw(segID);

        if (!sdw.IsValidWrapper())
        {
            printf("segmentation id %i is not a valid segmentation.\n", segID);
            return;
        }

        om::rebuilder::segmentation rebuilder(sdw);
        rebuilder.Rebuild();

    } else if(line.startsWith("exportSegReroot:")){
        QStringList args = line.split(':',QString::SkipEmptyParts);

        if (args.size() < 3)
        {
            printf("Please enter a segmentation id and export file name.\n");
            return;
        }

        const OmID segID = OmStringHelpers::getUInt(args[1]);

        SegmentationDataWrapper sdw(segID);

        if (!sdw.IsValidWrapper())
        {
            printf("segmentation id %i is not a valid segmentation.\n", segID);
            return;
        }

        HeadlessImpl::ExportAndRerootSegments(sdw, args[2]);

    } else if(line.startsWith("buildEmptyChannel")){
        ChannelDataWrapper cdw;
        OmChannel& chann = cdw.Create();

        OmBuildChannel bc(&chann);
        bc.BuildEmptyChannel();

    } else {
        printf("Could not parse \"%s\".\n", qPrintable(line));
    }
}

