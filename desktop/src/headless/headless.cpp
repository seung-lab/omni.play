#include "actions/omActions.h"
#include "chunks/omChunk.h"
#include "common/logging.h"
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

void Headless::runInteractive(const QString& fName) {
  QTextStream stream(stdin);
  QString line;
  do {
    printf("> ");
    line = stream.readLine();
    processLine(line, fName);
  } while (!line.isNull());
}

void Headless::runScript(const QString scriptFileName, const QString& fName) {
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

void Headless::RunHeadless(const QString& headlessCMD, const QString& fName) {
  if (fName != "") {
    HeadlessImpl::OpenProject(fName);
  }

  if ("" == headlessCMD) {
    runInteractive(fName);
  } else {
    runScript(headlessCMD, fName);
  }
}

#define OPTION(name)        \
  else if (#name == line) { \
    name();                 \
  }

#define OPTION_PASS(name)            \
  else if (line.startsWith(#name)) { \
    name(line);                      \
  }

void Headless::processLine(const QString& line, const QString&) {
  OmTimer timer;

  if (line == "q" || line == "quit") {
    printf("Exiting...\n");
    OmProject::Close();
    exit(0);
  }
  OPTION(save)
  OPTION(mesh)
  OPTION(DumpActions)
  OPTION(clearMST)
  OPTION(rebuildCenter)
  OPTION(revalidate)
  OPTION(loadDend)
  OPTION(loadChunk)
  OPTION(close)
  OPTION(pwd)
  OPTION(ls)
  OPTION(lsl)
  OPTION(lsChan)
  OPTION(lsSeg)
  OPTION(refindUniqueChunkValues)
  OPTION(dumpSegmentColors)
  OPTION(dumpRootSegmentColors)
  OPTION(recolorSegments)
  OPTION(buildEmptyChannel)
  OPTION_PASS(mesh)
  OPTION_PASS(compareChanns)
  OPTION_PASS(compareSegs)
  OPTION_PASS(dumpSegTiles)
  OPTION_PASS(dumpChannTiles)
  OPTION_PASS(seg)
  OPTION_PASS(open)
  OPTION_PASS(cd)
  OPTION_PASS(addSegment)
  OPTION_PASS(create)
  OPTION_PASS(createOrOpen)
  OPTION_PASS(loadHDF5seg)
  OPTION_PASS(loadHDF5chann)
  OPTION_PASS(loadHDF5affgraph)
  OPTION_PASS(loadTIFFchann)
  OPTION_PASS(loadTIFFseg)
  OPTION_PASS(buildHDF5)
  OPTION_PASS(removeChann)
  OPTION_PASS(removeSeg)
  OPTION_PASS(setChanResolution)
  OPTION_PASS(setSegResolution)
  OPTION_PASS(setChanAbsOffset)
  OPTION_PASS(setSegAbsOffset)
  OPTION_PASS(setMeshDownScallingFactor)
  OPTION_PASS(importWatershed)
  OPTION_PASS(timeSegChunkReads)
  OPTION_PASS(downsampleChan)
  OPTION_PASS(checkMeshes)
  OPTION_PASS(replaceChanSlice)
  OPTION_PASS(replaceSegSlice)
  OPTION_PASS(exportSegRaw)
  OPTION_PASS(rebuildSegmentation)
  OPTION_PASS(exportSegReroot)
  else if ("ls -l" == line) {
    lsl();
  }
  else {
    printf("Could not parse \"%s\".\n", qPrintable(line));
  }
}

#undef OPTION
#undef OPTION_PASS

void Headless::save() { OmActions::Save(); }

void Headless::mesh() {
  if (0 == segmentationID_) {
    printf("Please choose segmentation first!\n");
    return;
  }

  HeadlessImpl::Mesh(segmentationID_);
}

void Headless::DumpActions() { HeadlessImpl::DumpActionsToTextFile(); }

void Headless::mesh(const QString& line) {
  if (0 == segmentationID_) {
    printf("Please choose segmentation first!\n");
    return;
  }

  QStringList args = line.split(':', QString::SkipEmptyParts);

  if (args.size() < 2) {
    printf("Please specify threshold\n");
    return;
  }

  // const double threshold = OmStringHelpers::getDouble(args[1]);

  const SegmentationDataWrapper sdw(segmentationID_);
  OmBuildSegmentation bs(sdw);
  // bs.BuildMesh(threshold);
  assert(0 && "threshold option disabled");
}

void Headless::clearMST() {
  if (0 == segmentationID_) {
    printf("Please choose segmentation first!\n");
    return;
  }
  HeadlessImpl::ClearMST(segmentationID_);
}

void Headless::rebuildCenter() {
  if (0 == segmentationID_) {
    printf("Please choose segmentation first!\n");
    return;
  }
  HeadlessImpl::RebuildCenterOfSegmentData(segmentationID_);
}

void Headless::revalidate() {
  if (0 == segmentationID_) {
    printf("Please choose segmentation first!\n");
    return;
  }
  HeadlessImpl::ReValidateEveryObject(segmentationID_);
}

void Headless::loadDend() {
  if (0 == segmentationID_) {
    printf("Please choose segmentation first!\n");
    return;
  }
  const SegmentationDataWrapper sdw(segmentationID_);
  OmBuildSegmentation bs(sdw);
  bs.LoadDendrogram();
}

void Headless::compareChanns(const QString& line) {
  // format: compareChanns:id1,id2[:verbose]
  QStringList args = line.split(':', QString::SkipEmptyParts);

  if (args.size() < 2) {
    printf("Please specify channel IDs.\n");
    return;
  }

  QStringList channelIDs = args[1].split(',', QString::SkipEmptyParts);

  if (channelIDs.size() < 2) {
    printf("Not enough channel IDs specified. Please specify as 'id1,id2'.\n");
    return;
  }

  int id1 = OmStringHelpers::getUInt(channelIDs[0]);
  int id2 = OmStringHelpers::getUInt(channelIDs[1]);

  if (!ChannelDataWrapper(id1).IsChannelValid()) {
    printf("Channel %i is not a valid channel.\n", id1);
    return;
  }
  if (!ChannelDataWrapper(id2).IsChannelValid()) {
    printf("Channel %i is not a valid channel.\n", id2);
    return;
  }

  const bool same = OmCompareVolumes::CompareChannels(id1, id2);

  if (same) {
    printf("Channel %i and Channel %i are identical.\n", id1, id2);
  } else {
    printf("Channel %i and Channel %i are different!\n", id1, id2);
  }
}

void Headless::compareSegs(const QString& line) {
  // format: compareSegs:id1,id2[:verbose]
  QStringList args = line.split(':', QString::SkipEmptyParts);

  if (args.size() < 2) {
    printf("Please specify segmentation IDs.\n");
    return;
  }

  QStringList segmentationIDs = args[1].split(',', QString::SkipEmptyParts);

  if (segmentationIDs.size() < 2) {
    printf("Not enough segmentation IDs specified. Please specify as "
           "'id1,id2'.\n");
    return;
  }

  const int id1 = OmStringHelpers::getUInt(segmentationIDs[0]);
  const int id2 = OmStringHelpers::getUInt(segmentationIDs[1]);

  if (!SegmentationDataWrapper(id1).IsSegmentationValid()) {
    printf("Segmentation %i is not a valid segmentation.\n", id1);
    return;
  }
  if (!SegmentationDataWrapper(id2).IsSegmentationValid()) {
    printf("Segmentation %i is not a valid segmentation.\n", id2);
    return;
  }

  const bool same = OmCompareVolumes::CompareSegmentations(id1, id2);

  if (same) {
    printf("Segmentation %i and Segmentation %i are identical.\n", id1, id2);
  } else {
    printf("Segmentation %i and Segmentation %i are different!\n", id1, id2);
  }
}

void Headless::dumpSegTiles(const QString& line) {
  QStringList args = line.split(':', QString::SkipEmptyParts);

  QString fileNameAndPath = "/tmp/tiles.dump";
  om::common::ID segID = 1;

  if (3 == args.size()) {
    segID = OmStringHelpers::getUInt(args[1]);
    fileNameAndPath = args[2];
  } else if (2 == args.size()) {
    segID = 1;
    fileNameAndPath = args[1];
  }

  SegmentationDataWrapper sdw(segID);

  OmViewGroupState* vgs = new OmViewGroupState(NULL);
  OmTileDumper dumper(sdw.GetSegmentationPtr(), fileNameAndPath, vgs);
  dumper.DumpTiles();
}

void Headless::dumpChannTiles(const QString& line) {
  QStringList args = line.split(':', QString::SkipEmptyParts);
  QString file = "/tmp/tiles";
  om::common::ID chanID = 1;

  if (3 == args.size()) {
    chanID = OmStringHelpers::getUInt(args[1]);
    file = args[2];
  } else if (2 == args.size()) {
    chanID = 1;
    file = args[1];
  }

  ChannelDataWrapper cdw(chanID);

  OmViewGroupState* vgs = new OmViewGroupState(NULL);
  OmTileDumper dumper(cdw.GetChannelPtr(), file, vgs);
  dumper.DumpTiles();
}

void Headless::seg(const QString& line) {
  QStringList args = line.split(':', QString::SkipEmptyParts);
  if (args.size() < 2) {
    printf("Please enter a filename.\n");
    return;
  }
  segmentationID_ = OmStringHelpers::getUInt(args[1]);
  if (segmentationID_ > 0) {
    printf("segmentationID_ set to %d\n", segmentationID_);
  } else {
    printf("Invalid segmentation\n");
  }
}

void Headless::open(const QString& line) {
  QStringList args = line.split(':', QString::SkipEmptyParts);
  if (args.size() < 2) {
    printf("Please enter a filename.\n");
    return;
  }
  HeadlessImpl::OpenProject(args[1]);
}

void Headless::close() {
  OmProject::Close();
  printf("Project closed.\n");
}

void Headless::pwd() { printf("%s\n", qPrintable(QDir::currentPath())); }

void Headless::cd(const QString& line) {
  QStringList args = line.split("cd ", QString::SkipEmptyParts);
  if (args.size() < 1) {
    printf("Please enter a pathname.\n");
    return;
  }
  QString path = args[0];
  if (!QDir(path).exists()) {
    printf("Directory does not exist.\n");
    return;
  }
  QDir::setCurrent(path);
}

void Headless::ls() {
  QStringList entrylist = QDir::current().entryList();
  Q_FOREACH(QString str, entrylist) { printf("%s\n", qPrintable(str)); }
}

void Headless::lsl() {
  QFileInfoList entrylist = QDir::current().entryInfoList();
  Q_FOREACH(QFileInfo f, entrylist) {
    QString line = QString("%1%2").arg(f.fileName(), -20, ' ')
        .arg(QString::number((double) f.size() / om::math::bytesPerMB, 'f', 3));
    printf("%s\n", qPrintable(line));
  }
}

void Headless::addSegment(const QString& line) {
  if (0 == segmentationID_) {
    printf("Please choose segmentation first!\n");
    return;
  }
  SegmentationDataWrapper sdw(segmentationID_);
  sdw.Segments()->AddSegment();
  OmActions::Save();
}

void Headless::create(const QString& line) {
  QStringList args = line.split(':', QString::SkipEmptyParts);

  if (args.size() < 2) {
    printf("Please enter a filename.\n");
    return;
  }

  QString projectFileNameAndPath = args[1];
  const QString fname = OmProject::New(projectFileNameAndPath);
  RecentFileList::prependFileToFS(fname);
  printf("Created and opened %s.\n", qPrintable(fname));
}

void Headless::createOrOpen(const QString& line) {
  QStringList args = line.split(':', QString::SkipEmptyParts);

  if (args.size() < 2) {
    printf("Please enter a filename.\n");
    return;
  }

  QString projectFileNameAndPath = args[1];

  QFile file(projectFileNameAndPath);
  if (file.exists()) {
    OmProject::Load(projectFileNameAndPath);

  } else {
    OmProject::New(projectFileNameAndPath);
  }
}

void Headless::loadHDF5seg(const QString& line) {
  QStringList args = line.split(':', QString::SkipEmptyParts);

  if (args.size() < 2) {
    printf("Please enter a filename.\n");
    return;
  }

  HeadlessImpl::loadHDF5seg(args[1], segmentationID_);
}

void Headless::loadHDF5chann(const QString& line) {
  QStringList args = line.split(':', QString::SkipEmptyParts);

  if (args.size() < 2) {
    printf("Please enter a filename.\n");
    return;
  }

  QString hdf5fnp = args[1];

  OmBuildChannel bc;
  bc.addFileNameAndPath(hdf5fnp);
  bc.Build();
}

void Headless::loadHDF5affgraph(const QString& line) {
  QStringList args = line.split(':', QString::SkipEmptyParts);

  if (args.size() < 2) {
    printf("Please enter command like: loadHDF5affgraph:file.hdf5\n");
    return;
  }

  const QString& hdf5fnp = args[1];

  AffinityGraphDataWrapper agdw;
  auto& affGraph = agdw.Create();
  affGraph.ImportAllChannels(hdf5fnp);
}

void Headless::loadTIFFchann(const QString& line) {
  QStringList args = line.split(':', QString::SkipEmptyParts);

  if (args.size() < 2) {
    printf("Please enter a path to a directory.\n");
    return;
  }

  OmBuildChannel bc;

  QDir dir(args[1]);
  Q_FOREACH(QFileInfo f, dir.entryInfoList()) {
    if (!f.isFile()) {
      continue;
    }
    //printf("adding %s/\n", qPrintable(f.canonicalFilePath()));
    bc.addFileNameAndPath(f.canonicalFilePath());
  }
  bc.Build();
}

void Headless::loadTIFFseg(const QString& line) {
  QStringList args = line.split(':', QString::SkipEmptyParts);

  if (args.size() < 2) {
    printf("Please enter a path to a directory.\n");
    return;
  }

  OmBuildSegmentation bs;
  segmentationID_ = bs.GetDataWrapper().GetID();

  QDir dir(args[1]);

  Q_FOREACH(QFileInfo f, dir.entryInfoList()) {
    if (!f.isFile()) {
      continue;
    }
    //printf("adding %s/\n", qPrintable(f.canonicalFilePath()));
    bs.addFileNameAndPath(f.canonicalFilePath());
  }

  bs.BuildImage();
}

void Headless::buildHDF5(const QString& line) {
  QStringList args = line.split(':', QString::SkipEmptyParts);

  if (args.size() < 2) {
    printf("Please enter a filename.\n");
    return;
  }

  HeadlessImpl::buildHDF5(args[1]);
}

void Headless::loadChunk() {
  if (0 == segmentationID_) {
    printf("Please choose segmentation first!\n");
    return;
  }

  SegmentationDataWrapper sdw(segmentationID_);
  OmSegmentation& segmen = sdw.GetSegmentation();
  om::chunkCoord chunk_coord(0, 0, 0, 0);
  OmChunk* chunk = segmen.GetChunk(chunk_coord);
  assert(chunk);
}

void Headless::removeChann(const QString& line) {
  QStringList args = line.split(':', QString::SkipEmptyParts);

  if (args.size() < 2) {
    printf("Please enter a channel id.\n");
    return;
  }

  int channID = OmStringHelpers::getUInt(args[1]);

  if (!ChannelDataWrapper(channID).IsChannelValid()) {
    printf("Channel %i is not a valid channel.\n", channID);
    return;
  }

  ChannelDataWrapper::Remove(channID);
  printf("Channel %i removed.\n", channID);
}

void Headless::removeSeg(const QString& line) {
  QStringList args = line.split(':', QString::SkipEmptyParts);

  if (args.size() < 2) {
    printf("Please enter a segmentation id.\n");
    return;
  }

  int segID = OmStringHelpers::getUInt(args[1]);

  if (!SegmentationDataWrapper(segID).IsSegmentationValid()) {
    printf("Segmentation %i is not a valid segmentation.\n", segID);
    return;
  }

  SegmentationDataWrapper::Remove(segID);
  printf("Segmentation %i removed.\n", segID);
}

void Headless::lsChan() {
  const om::common::IDSet& chanset = ChannelDataWrapper::ValidIDs();

  if (chanset.empty()) {
    printf("No channels present.\n");
    return;
  }

  printf("ID\tName\n");

  FOR_EACH(iter, chanset) {
    ChannelDataWrapper cdw(*iter);
    printf("%i\t%s\n", *iter, qPrintable(cdw.GetName()));
  }
}

void Headless::lsSeg() {
  const om::common::IDSet& segset = SegmentationDataWrapper::ValidIDs();

  if (segset.empty()) {
    printf("No segmentations present.\n");
    return;
  }

  printf("ID\tName\n");

  FOR_EACH(iter, segset) {
    SegmentationDataWrapper sdw(*iter);
    printf("%i\t%s\n", *iter, qPrintable(sdw.GetName()));
  }
}

void Headless::setChanResolution(const QString& line) {
  const QStringList args = line.split(':', QString::SkipEmptyParts);

  if (args.size() != 2) {
    printf("format is setChannResolution:channID,xRes,yRes,zRes\n");
    return;
  }

  const QStringList res = args[1].split(',', QString::SkipEmptyParts);

  if (res.size() != 4) {
    printf("format is setChannResolution:channID,xRes,yRes,zRes\n");
    return;
  }

  const om::common::ID channID = OmStringHelpers::getUInt(res[0]);
  const float xRes = OmStringHelpers::getFloat(res[1]);
  const float yRes = OmStringHelpers::getFloat(res[2]);
  const float zRes = OmStringHelpers::getFloat(res[3]);

  ChannelDataWrapper cdw(channID);

  if (!cdw.IsChannelValid()) {
    printf("Channel %i is not a valid channel.\n", channID);
    return;
  }

  HeadlessImpl::ChangeVolResolution(cdw.GetChannel(), xRes, yRes, zRes);
}

void Headless::setSegResolution(const QString& line) {
  const QStringList args = line.split(':', QString::SkipEmptyParts);

  if (args.size() != 2) {
    printf("format is setSegResolution:channID,xRes,yRes,zRes\n");
    return;
  }

  const QStringList res = args[1].split(',', QString::SkipEmptyParts);

  if (res.size() != 4) {
    printf("format is setSegResolution:segID,xRes,yRes,zRes\n");
    return;
  }

  const om::common::ID segID = OmStringHelpers::getUInt(res[0]);
  const float xRes = OmStringHelpers::getFloat(res[1]);
  const float yRes = OmStringHelpers::getFloat(res[2]);
  const float zRes = OmStringHelpers::getFloat(res[3]);

  SegmentationDataWrapper sdw(segID);

  if (!sdw.IsSegmentationValid()) {
    printf("Segmentation %i is not a valid segmentation\n", segID);
    return;
  }

  HeadlessImpl::ChangeVolResolution(sdw.GetSegmentation(), xRes, yRes, zRes);
}

void Headless::setChanAbsOffset(const QString& line) {
  const QStringList args = line.split(':', QString::SkipEmptyParts);

  if (args.size() != 2) {
    printf("format is setChanAbsOffset:channID,xRes,yRes,zRes\n");
    return;
  }

  const QStringList res = args[1].split(',', QString::SkipEmptyParts);

  if (res.size() != 4) {
    printf("format is setChanAbsOffset:segID,xRes,yRes,zRes\n");
    return;
  }

  const om::common::ID segID = OmStringHelpers::getUInt(res[0]);
  const int xRes = OmStringHelpers::getUInt(res[1]);
  const int yRes = OmStringHelpers::getUInt(res[2]);
  const int zRes = OmStringHelpers::getUInt(res[3]);

  ChannelDataWrapper cdw(segID);

  if (!cdw.IsValidWrapper()) {
    printf("Channel %i is not a valid channel\n", segID);
    return;
  }

  HeadlessImpl::ChangeVolAbsOffset(cdw.GetChannel(), xRes, yRes, zRes);
}

void Headless::setSegAbsOffset(const QString& line) {
  const QStringList args = line.split(':', QString::SkipEmptyParts);

  if (args.size() != 2) {
    printf("format is setSegAbsOffset:channID,xRes,yRes,zRes\n");
    return;
  }

  const QStringList res = args[1].split(',', QString::SkipEmptyParts);

  if (res.size() != 4) {
    printf("format is setSegAbsOffset:segID,xRes,yRes,zRes\n");
    return;
  }

  const om::common::ID segID = OmStringHelpers::getUInt(res[0]);
  const int xRes = OmStringHelpers::getUInt(res[1]);
  const int yRes = OmStringHelpers::getUInt(res[2]);
  const int zRes = OmStringHelpers::getUInt(res[3]);

  SegmentationDataWrapper sdw(segID);

  if (!sdw.IsValidWrapper()) {
    printf("Segmentation %i is not a valid segmentation\n", segID);
    return;
  }

  HeadlessImpl::ChangeVolAbsOffset(sdw.GetSegmentation(), xRes, yRes, zRes);
}

void Headless::setMeshDownScallingFactor(const QString& line) {
  const QStringList args = line.split(':', QString::SkipEmptyParts);

  if (args.size() != 2) {
    printf("format is setMeshDownScallingFactor:factor (factor is double)\n");
    return;
  }

  const double factor = OmStringHelpers::getDouble(args[1]);

  HeadlessImpl::SetMeshDownScallingFactor(factor);
}

void Headless::dumpSegmentColors() {
  if (0 == segmentationID_) {
    printf("Please choose segmentation first!\n");
    return;
  }
  HeadlessImpl::DumpSegmentColorHistograms(segmentationID_);
}

void Headless::dumpRootSegmentColors() {
  if (0 == segmentationID_) {
    printf("Please choose segmentation first!\n");
    return;
  }
  HeadlessImpl::DumpRootSegmentColorHistograms(segmentationID_);
}

void Headless::recolorSegments() {
  if (0 == segmentationID_) {
    printf("Please choose segmentation first!\n");
    return;
  }
  HeadlessImpl::RecolorAllSegments(segmentationID_);
}

void Headless::refindUniqueChunkValues() {
  if (0 == segmentationID_) {
    printf("Please choose segmentation first!\n");
    return;
  }
  OmChunkUtils::RefindUniqueChunkValues(segmentationID_);

}

void Headless::importWatershed(const QString& line) {
  const QStringList args = line.split(':', QString::SkipEmptyParts);

  if (args.size() != 2) {
    printf("format is importWatershed:metadataFileName\n");
    return;
  }

  HeadlessImpl::ImportWatershed(args[1]);
}

void Headless::timeSegChunkReads(const QString& line) {
  const QStringList args = line.split(':', QString::SkipEmptyParts);

  if (args.size() != 4) {
    printf("format is timeSegChunkReads:segmentationID:randomize:useFseek\n");
    return;
  }

  const om::common::ID segID = OmStringHelpers::getUInt(args[1]);
  const bool randomizeOrder = OmStringHelpers::getBool(args[2]);
  const bool useRawChunk = OmStringHelpers::getBool(args[3]);

  HeadlessImpl::TimeSegChunkReads(segID, randomizeOrder, useRawChunk);
}

void Headless::downsampleChan(const QString& line) {
  QStringList args = line.split(':', QString::SkipEmptyParts);

  if (args.size() < 2) {
    printf("Please enter a channel id.\n");
    return;
  }

  int channID = OmStringHelpers::getUInt(args[1]);

  if (!ChannelDataWrapper(channID).IsChannelValid()) {
    printf("Channel %i is not a valid channel.\n", channID);
    return;
  }

  ChannelDataWrapper cdw(channID);
  if (cdw.IsValidWrapper()) {
    HeadlessImpl::DownsampleChannel(cdw);
  }
}

void Headless::checkMeshes(const QString& line) {
  QStringList args = line.split(':', QString::SkipEmptyParts);

  if (args.size() < 2) {
    printf("Please enter a segmentation id.\n");
    return;
  }

  const om::common::ID segID = OmStringHelpers::getUInt(args[1]);

  SegmentationDataWrapper sdw(segID);

  if (!sdw.IsValidWrapper()) {
    printf("segmentation id %i is not a valid segmentation.\n", segID);
    return;
  }

  HeadlessImpl::CheckMeshes(sdw);
}

void Headless::replaceChanSlice(const QString& line) {

  QStringList args = line.split(':', QString::SkipEmptyParts);

  if (args.size() < 4) {
    printf(
        "Please enter as replaceChanSlice::channelID::fileName::sliceNum.\n");
    return;
  }

  const om::common::ID channelID = OmStringHelpers::getUInt(args[1]);

  ChannelDataWrapper cdw(channelID);

  if (!cdw.IsValidWrapper()) {
    printf("channel id %i is not a valid channel.\n", channelID);
    return;
  }

  const QString fname = args[2];
  const int sliceNum = OmStringHelpers::getUInt(args[3]);

  HeadlessImpl::ReplaceSlice(cdw.GetChannelPtr(), fname, sliceNum);
}

void Headless::replaceSegSlice(const QString& line) {

  QStringList args = line.split(':', QString::SkipEmptyParts);

  if (args.size() < 4) {
    printf("Please enter as "
           "replaceSegSlice::segmentationID::fileName::sliceNum.\n");
    return;
  }

  const om::common::ID segmentationID = OmStringHelpers::getUInt(args[1]);

  SegmentationDataWrapper sdw(segmentationID);

  if (!sdw.IsValidWrapper()) {
    printf("segmentation id %i is not a valid segmentation.\n", segmentationID);
    return;
  }

  const QString fname = args[2];
  const int sliceNum = OmStringHelpers::getUInt(args[3]);

  HeadlessImpl::ReplaceSlice(sdw.GetSegmentationPtr(), fname, sliceNum);
}

void Headless::exportSegRaw(const QString& line) {
  QStringList args = line.split(':', QString::SkipEmptyParts);

  if (args.size() < 3) {
    printf("Please enter a segmentation id and export file name.\n");
    return;
  }

  const om::common::ID segID = OmStringHelpers::getUInt(args[1]);

  SegmentationDataWrapper sdw(segID);

  if (!sdw.IsValidWrapper()) {
    printf("segmentation id %i is not a valid segmentation.\n", segID);
    return;
  }

  HeadlessImpl::ExportSegmentationRaw(sdw, args[2]);
}

void Headless::rebuildSegmentation(const QString& line) {
  QStringList args = line.split(':', QString::SkipEmptyParts);

  if (args.size() < 2) {
    printf("Please enter a segmentation id.\n");
    return;
  }

  const om::common::ID segID = OmStringHelpers::getUInt(args[1]);

  SegmentationDataWrapper sdw(segID);

  if (!sdw.IsValidWrapper()) {
    printf("segmentation id %i is not a valid segmentation.\n", segID);
    return;
  }

  om::rebuilder::segmentation rebuilder(sdw);
  rebuilder.Rebuild();
}

void Headless::exportSegReroot(const QString& line) {
  QStringList args = line.split(':', QString::SkipEmptyParts);

  if (args.size() < 3) {
    printf("Please enter a segmentation id and export file name.\n");
    return;
  }

  const om::common::ID segID = OmStringHelpers::getUInt(args[1]);

  SegmentationDataWrapper sdw(segID);

  if (!sdw.IsValidWrapper()) {
    printf("segmentation id %i is not a valid segmentation.\n", segID);
    return;
  }

  HeadlessImpl::ExportAndRerootSegments(sdw, args[2]);
}

void Headless::buildEmptyChannel() {
  OmBuildChannel bc;
  bc.BuildEmptyChannel();
}
