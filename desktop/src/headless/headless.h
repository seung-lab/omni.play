#pragma once
#include "precomp.h"

#include "common/common.h"

class Headless {
 public:
  Headless() : segmentationID_(0) {}

  void RunHeadless(const QString& headlessCMD, const QString& fName);

 private:
  om::common::ID segmentationID_;

  void processLine(const QString& line, const QString& fName);
  void runInteractive(const QString& fName);
  void runScript(const QString scriptFileName, const QString& fName);
  void save();
  void mesh();
  void DumpActions();
  void clearMST();
  void rebuildCenter();
  void revalidate();
  void loadDend();
  void loadChunk();
  void close();
  void pwd();
  void ls();
  void lsl();
  void lsChan();
  void lsSeg();
  void refindUniqueChunkValues();
  void dumpSegmentColors();
  void dumpRootSegmentColors();
  void recolorSegments();
  void buildEmptyChannel();
  void mesh(const QString&);
  void compareChanns(const QString&);
  void compareSegs(const QString&);
  void dumpSegTiles(const QString&);
  void dumpChannTiles(const QString&);
  void seg(const QString&);
  void open(const QString&);
  void cd(const QString&);
  void addSegment(const QString&);
  void create(const QString&);
  void createOrOpen(const QString&);
  void loadHDF5seg(const QString&);
  void loadHDF5chann(const QString&);
  void loadHDF5affgraph(const QString&);
  void loadTIFFchann(const QString&);
  void loadTIFFseg(const QString&);
  void buildHDF5(const QString&);
  void removeChann(const QString&);
  void removeSeg(const QString&);
  void setChanResolution(const QString&);
  void setSegResolution(const QString&);
  void setChanAbsOffset(const QString&);
  void setSegAbsOffset(const QString&);
  void setMeshDownScallingFactor(const QString&);
  void importWatershed(const QString&);
  void timeSegChunkReads(const QString&);
  void downsampleChan(const QString&);
  void checkMeshes(const QString&);
  void replaceChanSlice(const QString&);
  void replaceSegSlice(const QString&);
  void exportSegRaw(const QString&);
  void rebuildSegmentation(const QString&);
  void exportSegReroot(const QString&);
};
