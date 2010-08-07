#include "gui/mstViewer.hpp"
#include "volume/omSegmentation.h"
#include "segment/omSegmentCache.h"

MstViewerImpl::MstViewerImpl(QWidget * parent, SegmentationDataWrapper sdw)
	: QTableWidget(parent)
	, sdw_(sdw)
{
	populate();
}

void MstViewerImpl::populate()
{
	OmSegmentation & segmentation = sdw_.getSegmentation();

	const int numEdges = segmentation.mDendCount;
	const quint32 * nodes    = segmentation.mDend->getQuint32Ptr();
	const float * thresholds = segmentation.mDendValues->getFloatPtr();
	/*
	quint8 * edgeDisabledByUse = segmentation.mEdgeDisabledByUser->getQuint8Ptr();
	quint8 * edgeWasJoined = segmentation.mEdgeWasJoined->getQuint8Ptr();
	quint8 * edgeForceJoin = segmentation.mEdgeForceJoin->getQuint8Ptr();
	const float stopThreshold = segmentation.mDendThreshold;
	*/

	QStringList headerLabels;
	headerLabels << "Edge" << "Node 1" << "Node 2" << "threshold" << "Node 1 size" << "Node 2 size";
	setColumnCount(headerLabels.size());
	setHorizontalHeaderLabels(headerLabels);

	setRowCount(numEdges);

	for( int i = 0; i < numEdges; ++i){
		int colNum = 0;
		const OmSegID node1ID  = nodes[i];
		const OmSegID node2ID  = nodes[i + numEdges ];
		const float threshold  = thresholds[i];

		OmSegment* node1 = segmentation.GetSegmentCache()->GetSegment(node1ID);
		OmSegment* node2 = segmentation.GetSegmentCache()->GetSegment(node2ID);

		add(i, colNum, i);
		add(i, colNum, node1ID);
		add(i, colNum, node2ID);
		add(i, colNum, threshold);
		add(i, colNum, node1->getSize());
		add(i, colNum, node2->getSize());
	}

	setSortingEnabled(true); // don't enable sorting until done inserting
	sortItems(0); // sort by edge number
}
