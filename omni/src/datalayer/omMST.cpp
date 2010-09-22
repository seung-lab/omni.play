#include "datalayer/omDataLayer.h"
#include "datalayer/omDataPath.h"
#include "datalayer/omIDataReader.h"
#include "datalayer/omDataWrapper.h"
#include "datalayer/omIDataWriter.h"
#include "datalayer/omMST.h"
#include "project/omProject.h"
#include "segment/lowLevel/DynamicForestPool.hpp"
#include "segment/omSegmentCache.h"
#include "system/omProjectData.h"
#include "utility/omSmartPtr.hpp"
#include "utility/stringHelpers.h"
#include "volume/omSegmentation.h"

static const float DefaultThresholdSize = 0.5;

OmMST::OmMST()
  : mDendSize(0)
  , mDendValuesSize(0)
  , mDendCount(0)
  , mDendThreshold(DefaultThresholdSize)
  , valid_(false)
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

  if(!OmProjectData::GetProjectIDataReader()->dataset_exists(path)) {
	std::cout << "did not find MST at path \""
			  << path.getString().c_str()
			  << "\"\n";
    return;
  }

  int size;
  mDend = OmProjectData::GetProjectIDataReader()->readDataset(path, &size);
  if( size != mDendSize ){
    printf("warning: something may be bad...\n");
    printf("dend: size=%d, mDendSize=%d\n", size, mDendSize);
    printf("type is %s\n", mDend->getTypeAsString().c_str());
    assert( size == mDendSize );
  }

  path = getDendValuesPath(seg);
  mDendValues = OmProjectData::GetProjectIDataReader()->readDataset(path, &size);
  if( size != mDendValuesSize ){
    printf("warning: something may be bad...\n");
    printf("dendValues: size=%d, mDendValuesSize=%d\n", size, mDendValuesSize);
    printf("type is %s\n", mDendValues->getTypeAsString().c_str());
    assert( size == mDendValuesSize );
  }

  path = getEdgeDisabledByUserPath(seg);
  mEdgeDisabledByUser = OmProjectData::GetProjectIDataReader()->readDataset(path, &size);
  if( size != mDendValuesSize ){
    printf("warning: something may be bad...\n");
    printf("dendEdgeDisabledByUser: size=%d, mDendValuesSize=%d\n", size, mDendValuesSize);
    printf("type is %s\n", mEdgeDisabledByUser->getTypeAsString().c_str());
    assert( size == mDendValuesSize );
  }

  // this is just a temporary object--should be refactored... (purcaro)
  mEdgeWasJoined = OmSmartPtr<uint8_t>::makeMallocPtrNumElements(mDendValuesSize);

  path = getEdgeForceJoinPath(seg);
  mEdgeForceJoin = OmProjectData::GetProjectIDataReader()->readDataset(path, &size);
  if( size != mDendValuesSize ){
    printf("warning: something may be bad...\n");
    printf("dendEdgeForceJoin: size=%d, mDendValuesSize=%d\n", size, mDendValuesSize);
    printf("type is %s\n", mEdgeForceJoin->getTypeAsString().c_str());
    assert( size == mDendValuesSize );
  }

  valid_ = true;
}

void OmMST::import(OmSegmentation & seg, const QString fname)
{
  OmIDataReader* hdf5reader = OmDataLayer::getReader(fname, true);
  hdf5reader->open();

  if(!importDend(hdf5reader)){
    return;
  }

  if(!importDendValues(hdf5reader)){
    return;
  }

  //	assert( vSize.x == dSize.y );

  setupUserEdges(mDendValuesSize);

  seg.FlushDend();

  hdf5reader->close();

  valid_ = true;
}

bool OmMST::importDend(OmIDataReader* hdf5reader)
{
  OmDataPath fpath;
  fpath.setPathQstr("dend");
  if( !hdf5reader->dataset_exists(fpath)){
    printf("no dendrogram dataset found\n");
    return false;
  }
  Vector3 < int > dSize = hdf5reader->getDatasetDims(fpath);
  int dendSize;
  OmDataWrapperPtr dend = hdf5reader->readDataset(fpath, &dendSize);
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

bool OmMST::importDendValues(OmIDataReader * hdf5reader)
{
  OmDataPath fpath;
  fpath.setPathQstr("dendValues");
  if(!hdf5reader->dataset_exists(fpath)){
    printf("no dendrogram values dataset found\n");
    return false;
  }
  Vector3 < int > vSize = hdf5reader->getDatasetDims(fpath);
  int dendValuesSize;
  OmDataWrapperPtr dendValues = hdf5reader->readDataset(fpath, &dendValuesSize);

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
  boost::shared_ptr<uint8_t> edbu =
	  OmSmartPtr<uint8_t>::makeMallocPtrNumElements(dendValuesSize,
							OM::ZERO_FILL);
  mEdgeDisabledByUser = OmDataWrapperFactory::produce(edbu);

  boost::shared_ptr<uint8_t> efj =
	  OmSmartPtr<uint8_t>::makeMallocPtrNumElements(dendValuesSize,
							OM::ZERO_FILL);
  mEdgeForceJoin = OmDataWrapperFactory::produce(efj);

  mEdgeWasJoined = OmSmartPtr<uint8_t>::makeMallocPtrNumElements(dendValuesSize);

  return true;
}

void OmMST::FlushDend(OmSegmentation * seg)
{
  OmDataPath path = getDendPath(*seg);
  printf("dend: will save %s bytes\n",
	 qPrintable(StringHelpers::commaDeliminateNumber(mDendSize)));
  OmProjectData::GetIDataWriter()->
    writeDataset(path,
				      mDendSize,
				      mDend);

  path = getDendValuesPath(*seg);
  printf("dendValues: will save %s bytes\n",
	 qPrintable(StringHelpers::commaDeliminateNumber(mDendValuesSize)));
  OmProjectData::GetIDataWriter()->
    writeDataset(path,
		 mDendValuesSize,
		 mDendValues);

  FlushDendUserEdges(seg);
}

void OmMST::FlushDendUserEdges(OmSegmentation * seg)
{
  OmDataPath path(getEdgeDisabledByUserPath(*seg));

  OmProjectData::GetIDataWriter()->
    writeDataset(path,
		 mDendValuesSize,
		 mEdgeDisabledByUser);

  path = getEdgeForceJoinPath(*seg);
  OmProjectData::GetIDataWriter()->
    writeDataset(path,
		 mDendValuesSize,
		 mEdgeForceJoin);
}
