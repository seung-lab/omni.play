#include "datalayer/omDataLayer.h"
#include "datalayer/omDataPath.h"
#include "datalayer/omIDataReader.h"
#include "datalayer/omDataWrapper.h"
#include "datalayer/omIDataWriter.h"
#include "project/omProject.h"
#include "segment/io/omMST.h"
#include "segment/io/omMSTold.h"
#include "segment/omSegmentCache.h"
#include "system/omProjectData.h"
#include "utility/omSmartPtr.hpp"
#include "utility/stringHelpers.h"
#include "volume/omSegmentation.h"

static const double DefaultThresholdSize = 0.5;

OmMST::OmMST(OmSegmentation* segmentation)
	: segmentation_(segmentation)
	, numEdges_(0)
	, userThreshold_(DefaultThresholdSize)
	, edges_(NULL)
{}

QString OmMST::memMapPathQStr()
{
	const QString volPath = OmFileNames::MakeVolSegmentsPath(segmentation_);
	return QString("%1mst.data").arg(volPath);
}

std::string OmMST::memMapPath(){
	return memMapPathQStr().toStdString();
}

void OmMST::Read()
{
	if(!numEdges_){
		printf("no MST found\n");
		return;
	}

	edgesPtr_ = reader_t::Reader(memMapPath());
	edges_ = edgesPtr_->GetPtr();

	const uint64_t expectedSize = numEdges_*sizeof(OmMSTEdge);
	if( expectedSize != edgesPtr_->Size()){
		const QString err =
			QString("mst sizes did not match: file was %1, but expected %2")
			.arg(edgesPtr_->Size())
			.arg(expectedSize);
		throw OmIoException(err.toStdString());
	}

	for(uint32_t i = 0; i < numEdges_; ++i){
		edges_[i].wasJoined = 0; // always zero out
	}
}

void OmMST::create()
{
	assert(numEdges_);

	edgesPtr_ = writer_t::WriterNumElements(memMapPath(),
										   numEdges_,
										   om::ZERO_FILL);

	edges_ = edgesPtr_->GetPtr();
}

void OmMST::convert()
{
	OmMSTold old;
	old.readOld(*segmentation_);
	// numEdges_ already set by OmDataArchiveProject

	doReadInFromOldMST(old);
}

void OmMST::import(const std::string& fname)
{
	OmMSTold old;
	old.import(fname);
	numEdges_ = old.NumEdges();

	doReadInFromOldMST(old);
}

void OmMST::doReadInFromOldMST(const OmMSTold& old)
{
	if(!numEdges_){
		printf("no MST found\n");
		return;
	}
	if(!old.mDend || !old.mDendValues || !old.mEdgeDisabledByUser || !old.mEdgeForceJoin) {
		printf("old MST not populated\n");
		return;
	}

	create();

	const quint32 * nodes = old.mDend->getPtr<uint32_t>();
	const float * thresholds = old.mDendValues->getPtr<float>();
	uint8_t* edgeDisabledByUser = old.mEdgeDisabledByUser->getPtr<uint8_t>();
	uint8_t* edgeForceJoin = old.mEdgeForceJoin->getPtr<uint8_t>();

	for(uint32_t i = 0; i < numEdges_; ++i){
		edges_[i].number = i;
		edges_[i].node1ID = nodes[i];
		edges_[i].node2ID = nodes[i + numEdges_ ];
		edges_[i].threshold = thresholds[i];
		edges_[i].userSplit = edgeDisabledByUser[i];
		edges_[i].userJoin = edgeForceJoin[i];
		edges_[i].wasJoined = 0; // always zero out
	}

	Flush();
}

void OmMST::Flush()
{
	if(numEdges_){
		edgesPtr_->Flush();
	}
}
