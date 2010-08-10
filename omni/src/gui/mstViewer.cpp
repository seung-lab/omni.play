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
	OmMST & mst = segmentation.mst;

	const int numEdges = mst.mDendCount;
	const quint32 * nodes    = mst.mDend->getPtr<unsigned int>();
	const float * thresholds = mst.mDendValues->getPtr<float>();

	QStringList headerLabels;
	headerLabels << "Edge" << "Node 1" << "Node 2" << "threshold" << "Node 1 size" << "Node 2 size";
	setColumnCount(headerLabels.size());
	setHorizontalHeaderLabels(headerLabels);

	setRowCount(numEdges);

	for( int i = 0; i < numEdges; ++i){
		const OmSegID node1ID  = nodes[i];
		const OmSegID node2ID  = nodes[i + numEdges ];
		const float threshold  = thresholds[i];

		OmSegment* node1 = segmentation.GetSegmentCache()->GetSegment(node1ID);
		OmSegment* node2 = segmentation.GetSegmentCache()->GetSegment(node2ID);

		int colNum = 0;
		setCell(i, colNum, i);
		setCell(i, colNum, node1ID);
		setCell(i, colNum, node2ID);
		setCell(i, colNum, threshold);
		setCell(i, colNum, node1->getSize());
		setCell(i, colNum, node2->getSize());
	}

	setSortingEnabled(true); // don't enable sorting until done inserting
	sortItems(0); // sort by edge number
}
