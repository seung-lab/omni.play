#include "datalayer/archive/old/utilsOld.hpp"
#include "datalayer/archive/old/omMipVolumeArchive.h"
#include "project/omProject.h"

QDataStream& operator<<(QDataStream& out, const om::coords::VolumeSystem& c) {
  // out << c.normToDataMat_;
  // out << c.normToDataInvMat_;
  // out << c.DataDimensions();
  // out << c.dataResolution_;
  // out << c.ChunkDimensions().x;

  // out << c.mMipLeafDim;
  // out << c.mMipRootLevel;

  return out;
}

QDataStream& operator>>(QDataStream& in, om::coords::VolumeSystem& c) {
  Matrix4f dummyMat;
  in >> dummyMat;  // normToDataMat_
  in >> dummyMat;  // normToDataInvMat_
  AxisAlignedBoundingBox<int> extent;
  in >> extent;
  c.SetDataDimensions(extent.getDimensions());
  Vector3f resolution;
  in >> resolution;
  c.SetResolution(resolution);
  int chunkDim;
  in >> chunkDim;
  c.SetChunkDimensions(Vector3i(chunkDim));
  QString dummyQString;
  in >> dummyQString;  // c.unitString_
  Vector3f dummyVec;
  in >> dummyVec;  // c.dataStretchValues_

  int dummyInt;
  in >> dummyInt;
  in >> dummyInt;

  return in;
}
