#include "segment/omSegmentCache.h"
#include "segment/lowLevel/DynamicForestPool.hpp"
#include "datalayer/omDataPath.h"
#include "datalayer/omDataLayer.h"
#include "datalayer/omDataReader.h"
#include "datalayer/omDataWrapper.h"
#include "datalayer/omDataWriter.h"
#include "datalayer/omMST.h"
#include "project/omProject.h"
#include "system/omProjectData.h"
#include "volume/omSegmentation.h"
#include "utility/stringHelpers.h"

static const float DefaultThresholdSize = 0.5;

OmMST::OmMST()
  : mDendSize(0)
  , mDendValuesSize(0)
  , mDendCount(0)
  , mDendThreshold(DefaultThresholdSize)
  , valid(false)
{
}

OmDataPath OmMST::getDendPath(OmSegmentation & seg) {
	return OmDataPath(seg.GetDirectoryPath() + "dend");
}
OmDataPath OmMST::getDendValuesPath(OmSegmentation & seg) {
	return OmDataPath(seg.GetDirectoryPath() + "dendValues");
}
OmDataPath OmMST::getEdgeDisabledByUserPath(OmSegmentation & seg){
	return OmDataPath(seg.GetDirectoryPath() + "/edgeDisabledByUser");
};
OmDataPath OmMST::getEdgeForceJoinPath(OmSegmentation & seg){
	return OmDataPath(seg.GetDirectoryPath() + "/edgeForceJoin");
};

void OmMST::read(OmSegmentation & seg)
{
  OmDataPath path(getDendPath(seg));

  if(!OmProjectData::GetProjectDataReader()->dataset_exists(path)) {
    return;
  }

  int size;
  mDend = OmProjectData::GetProjectDataReader()->dataset_raw_read(path, &size);
  if( size != mDendSize ){
    printf("warning: something may be bad...\n");
    printf("dend: size=%d, mDendSize=%d\n", size, mDendSize);
    printf("type is %s\n", mDend->getTypeAsString().c_str());
    assert( size == mDendSize );
  }

  path = getDendValuesPath(seg);
  mDendValues = OmProjectData::GetProjectDataReader()->dataset_raw_read(path, &size);
  if( size != mDendValuesSize ){
    printf("warning: something may be bad...\n");
    printf("dendValues: size=%d, mDendValuesSize=%d\n", size, mDendValuesSize);
    printf("type is %s\n", mDendValues->getTypeAsString().c_str());
    assert( size == mDendValuesSize );
  }

  path = getEdgeDisabledByUserPath(seg);
  mEdgeDisabledByUser = OmProjectData::GetProjectDataReader()->dataset_raw_read(path, &size);
  if( size != mDendValuesSize ){
    printf("warning: something may be bad...\n");
    printf("dendEdgeDisabledByUser: size=%d, mDendValuesSize=%d\n", size, mDendValuesSize);
    printf("type is %s\n", mEdgeDisabledByUser->getTypeAsString().c_str());
    assert( size == mDendValuesSize );
  }

  // this is just a temporary object--should be refactored... (purcaro)
  quint8 * edgeJoined = (quint8 *)malloc(sizeof(quint8) * mDendValuesSize );
  memset(edgeJoined, 0, sizeof(quint8) * mDendValuesSize );
  mEdgeWasJoined = OmDataWrapper<unsigned char>::produce(edgeJoined);

  path = getEdgeForceJoinPath(seg);
  mEdgeForceJoin = OmProjectData::GetProjectDataReader()->dataset_raw_read(path, &size);
  if( size != mDendValuesSize ){
    printf("warning: something may be bad...\n");
    printf("dendEdgeForceJoin: size=%d, mDendValuesSize=%d\n", size, mDendValuesSize);
    printf("type is %s\n", mEdgeForceJoin->getTypeAsString().c_str());
    assert( size == mDendValuesSize );
  }

  valid = true;
}

void OmMST::import(OmSegmentation & seg, const QString fname)
{
  OmDataLayer dl;
  OmDataReader * hdf5reader = dl.getReader(fname, true);
  hdf5reader->open();

  if(!importDend(hdf5reader)){
    return;
  }

  if(!importDendValues(hdf5reader)){
    return;
  }

  //	assert( vSize.x == dSize.y );

  setupUserEdges(mDendValuesSize);

  convertToEdgeList( seg,
		     mDend->getPtr<unsigned int>(),
		     mDendValues->getPtr<float>(),
		     mDendCount );

  seg.FlushDend();

  hdf5reader->close();

  valid = true;
}

bool OmMST::importDend(OmDataReader * hdf5reader)
{
  OmDataPath fpath;
  fpath.setPathQstr("dend");
  if( !hdf5reader->dataset_exists(fpath)){
    printf("no dendrogram dataset found\n");
    return false;
  }
  Vector3 < int > dSize = hdf5reader->dataset_get_dims(fpath);
  int dendSize;
  OmDataWrapperPtr dend = hdf5reader->dataset_raw_read(fpath, &dendSize);
  printf("\tdendrogram is %s x %s (%s bytes)\n",
	 qPrintable(StringHelpers::commaDeliminateNumber(dSize.x)),
	 qPrintable(StringHelpers::commaDeliminateNumber(dSize.y)),
	 qPrintable(StringHelpers::commaDeliminateNumber(dendSize)));

  assert( 2 == dSize.x );
  mDendCount = dSize.y;
  mDend = dend;
  mDendSize = dendSize;

  return true;
}

bool OmMST::importDendValues(OmDataReader * hdf5reader)
{
  OmDataPath fpath;
  fpath.setPathQstr("dendValues");
  if(!hdf5reader->dataset_exists(fpath)){
    printf("no dendrogram values dataset found\n");
    return false;
  }
  Vector3 < int > vSize = hdf5reader->dataset_get_dims(fpath);
  int dendValuesSize;
  OmDataWrapperPtr dendValues = hdf5reader->dataset_raw_read(fpath, &dendValuesSize);

  printf("\tdendrogram values is %s x %s (%s bytes)\n",
	 qPrintable(StringHelpers::commaDeliminateNumber(vSize.x)),
	 qPrintable(StringHelpers::commaDeliminateNumber(vSize.y)),
	 qPrintable(StringHelpers::commaDeliminateNumber(dendValuesSize)));

  assert( 0 == vSize.y );
  mDendValues = dendValues;
  mDendValuesSize = dendValuesSize;

  return true;
}

bool OmMST::setupUserEdges(const int dendValuesSize)
{
  quint8 * userDisabledEdge = (quint8 *)malloc(sizeof(quint8) * dendValuesSize );
  memset(userDisabledEdge, 0, sizeof(quint8) * dendValuesSize );
  OmDataWrapperPtr edgeDisabledByUser = OmDataWrapper<unsigned char>::produce(userDisabledEdge);

  // this is just a temporary object--should be refactored... (purcaro)
  quint8 * edgeJoined = (quint8 *)malloc(sizeof(quint8) * dendValuesSize );
  memset(edgeJoined, 0, sizeof(quint8) * dendValuesSize );
  OmDataWrapperPtr edgeWasJoined = OmDataWrapper<unsigned char>::produce(edgeJoined);

  quint8 * edgeForce = (quint8 *)malloc(sizeof(quint8) * dendValuesSize );
  memset(edgeForce, 0, sizeof(quint8) * dendValuesSize );
  OmDataWrapperPtr edgeForceJoin = OmDataWrapper<unsigned char>::produce(edgeForce);

  mEdgeDisabledByUser = edgeDisabledByUser;
  mEdgeForceJoin = edgeForceJoin;
  mEdgeWasJoined = edgeWasJoined;

  return true;
}

// rewrite child node IDs in MST, converting it to edge list
void OmMST::convertToEdgeList( OmSegmentation & seg,
			       quint32 * dend,
			       float * dendValues,
			       const int numDendRows )
{
  const OmSegID maxSegValue =  seg.GetSegmentCache()->getMaxValue() + 1;
  zi::DynamicForestPool<OmSegID> graph(maxSegValue);

  OmSegID childUnknownDepthID;
  OmSegID childRootID;
  OmSegID parentID;
  float threshold;
  int numBadSegValues = 0;

  for(int i = 0; i < numDendRows; ++i) {
    childUnknownDepthID = dend[i];
    parentID = dend[i + numDendRows ];
    threshold = dendValues[i];

    // Data may have values that don't exist in the volume... warn user.
    if(childUnknownDepthID < maxSegValue &&
       parentID < maxSegValue) {
      childRootID = graph.root(childUnknownDepthID);
      graph.join(childRootID, parentID);

      // set child ID to root value found by graph...
      dend[i] = childRootID;
    } else {
      ++numBadSegValues;
    }
  }

  if(0 != numBadSegValues){
    printf("warning: dend has %d values that don't exist in the volume data.\n", numBadSegValues);
  }
}

void OmMST::FlushDend(OmSegmentation * seg)
{
  OmDataPath path = getDendPath(*seg);
  printf("dend: will save %s bytes\n",
	 qPrintable(StringHelpers::commaDeliminateNumber(mDendSize)));
  OmProjectData::GetDataWriter()->
    dataset_raw_create_tree_overwrite(path,
				      mDendSize,
				      mDend);

  path = getDendValuesPath(*seg);
  printf("dendValues: will save %s bytes\n",
	 qPrintable(StringHelpers::commaDeliminateNumber(mDendValuesSize)));
  OmProjectData::GetDataWriter()->
    dataset_raw_create_tree_overwrite(path,
				      mDendValuesSize,
				      mDendValues);

  FlushDendUserEdges(seg);
}

void OmMST::FlushDendUserEdges(OmSegmentation * seg)
{
  OmDataPath path(getEdgeDisabledByUserPath(*seg));

  OmProjectData::GetDataWriter()->
    dataset_raw_create_tree_overwrite(path,
				      mDendValuesSize,
				      mEdgeDisabledByUser);

  path = getEdgeForceJoinPath(*seg);
  OmProjectData::GetDataWriter()->
    dataset_raw_create_tree_overwrite(path,
				      mDendValuesSize,
				      mEdgeForceJoin);
}
