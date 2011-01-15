#include "datalayer/omDataLayer.h"
#include "datalayer/omDataPath.h"
#include "datalayer/omIDataReader.h"
#include "datalayer/omDataWrapper.h"
#include "datalayer/omIDataWriter.h"
#include "segment/io/omMSTold.h"
#include "project/omProject.h"
#include "segment/lowLevel/DynamicForestPool.hpp"
#include "segment/omSegmentCache.h"
#include "system/omProjectData.h"
#include "utility/omSmartPtr.hpp"
#include "utility/omStringHelpers.h"
#include "volume/omSegmentation.h"

static const float DefaultThresholdSize = 0.5;

OmMSTold::OmMSTold()
	:mDendCount(0)
{}

OmDataPath OmMSTold::getDendPath(OmSegmentation & seg) {
	return OmDataPath(seg.GetDirectoryPath() + "dend");
}

OmDataPath OmMSTold::getDendValuesPath(OmSegmentation & seg) {
	return OmDataPath(seg.GetDirectoryPath() + "dendValues");
}

OmDataPath OmMSTold::getEdgeDisabledByUserPath(OmSegmentation & seg){
	return OmDataPath(seg.GetDirectoryPath() + "/edgeDisabledByUser");
};

OmDataPath OmMSTold::getEdgeForceJoinPath(OmSegmentation & seg){
	return OmDataPath(seg.GetDirectoryPath() + "/edgeForceJoin");
};

void OmMSTold::readOld(OmSegmentation & seg)
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

	path = getDendValuesPath(seg);
	mDendValues = OmProjectData::GetProjectIDataReader()->readDataset(path, &size);

	path = getEdgeDisabledByUserPath(seg);
	mEdgeDisabledByUser = OmProjectData::GetProjectIDataReader()->readDataset(path, &size);

	path = getEdgeForceJoinPath(seg);
	mEdgeForceJoin = OmProjectData::GetProjectIDataReader()->readDataset(path, &size);
}

void OmMSTold::import(const std::string& fname)
{
	OmIDataReader* hdf5reader = OmDataLayer::getReader(fname, true);
	hdf5reader->open();

	if(!importDend(hdf5reader)){
		return;
	}

	if(!importDendValues(hdf5reader)){
		return;
	}

	hdf5reader->close();

	setupUserEdges();
}

void OmMSTold::setupUserEdges()
{
	boost::shared_ptr<uint8_t> edbu =
		OmSmartPtr<uint8_t>::MallocNumElements(mDendCount,
											   om::ZERO_FILL);
	mEdgeDisabledByUser = OmDataWrapperFactory::produce(edbu);

	boost::shared_ptr<uint8_t> efj =
		OmSmartPtr<uint8_t>::MallocNumElements(mDendCount,
											   om::ZERO_FILL);
	mEdgeForceJoin = OmDataWrapperFactory::produce(efj);
}

bool OmMSTold::importDend(OmIDataReader* hdf5reader)
{
	OmDataPath fpath;
	fpath.setPathQstr("dend");
	if( !hdf5reader->dataset_exists(fpath)){
		printf("no dendrogram dataset found\n");
		return false;
	}

	const Vector3i dSize = hdf5reader->getDatasetDims(fpath);
	int dendSize;
	mDend = hdf5reader->readDataset(fpath, &dendSize);
	printf("\tdendrogram is %s x %s (%s bytes)\n",
		   OmStringHelpers::CommaDeliminateNum(dSize.x).c_str(),
		   OmStringHelpers::CommaDeliminateNum(dSize.y).c_str(),
		   OmStringHelpers::CommaDeliminateNum(dendSize).c_str());

	mDendCount = dSize.y;

	return true;
}

bool OmMSTold::importDendValues(OmIDataReader * hdf5reader)
{
	OmDataPath fpath;
	fpath.setPathQstr("dendValues");
	if(!hdf5reader->dataset_exists(fpath)){
		printf("no dendrogram values dataset found\n");
		return false;
	}

	const Vector3i vSize = hdf5reader->getDatasetDims(fpath);
	int dendValuesSize;
	mDendValues = hdf5reader->readDataset(fpath, &dendValuesSize);

	printf("\tdendrogram values is %s x %s (%s bytes)\n",
		   OmStringHelpers::CommaDeliminateNum(vSize.x).c_str(),
		   OmStringHelpers::CommaDeliminateNum(vSize.y).c_str(),
		   OmStringHelpers::CommaDeliminateNum(dendValuesSize).c_str());

	return true;
}
