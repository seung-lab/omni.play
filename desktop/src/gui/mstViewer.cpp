#include "utility/dataWrappers.h"
#include "gui/mstViewer.hpp"
#include "volume/omSegmentation.h"
#include "segment/omSegments.h"
#include "segment/io/omMST.h"

MstViewerImpl::MstViewerImpl(QWidget* parent, SegmentationDataWrapper sdw)
    : QTableWidget(parent), sdw_(sdw) {
  populate();
}

void MstViewerImpl::populate() {
  OmSegmentation& segmentation = sdw_.GetSegmentation();
  OmMST* mst = segmentation.MST();

  const int numEdges = mst->NumEdges();
  OmMSTEdge* edges = mst->Edges();

  QStringList headerLabels;
  headerLabels << "Edge"
               << "Node 1"
               << "Node 2"
               << "threshold"
               << "Node 1 size"
               << "Node 2 size";
  setColumnCount(headerLabels.size());
  setHorizontalHeaderLabels(headerLabels);

  setRowCount(numEdges);

  for (int i = 0; i < numEdges; ++i) {
    const OmSegID node1ID = edges[i].node1ID;
    const OmSegID node2ID = edges[i].node2ID;
    const float threshold = edges[i].threshold;

    OmSegment* node1 = segmentation.Segments()->GetSegment(node1ID);
    OmSegment* node2 = segmentation.Segments()->GetSegment(node2ID);

    int colNum = 0;
    setCell(i, colNum, i);
    setCell(i, colNum, node1ID);
    setCell(i, colNum, node2ID);
    setCell(i, colNum, threshold);
    setCell(i, colNum, static_cast<quint64>(node1->size()));
    setCell(i, colNum, static_cast<quint64>(node2->size()));
  }

  setSortingEnabled(true);  // don't enable sorting until done inserting
  sortItems(0);             // sort by edge number
}
