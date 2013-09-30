#include "gui/inspectors/segmentation/toolsPage/buttons/addSegmentButton.h"
#include "gui/inspectors/segmentation/toolsPage/pageTools.h"
#include "utility/omStringHelpers.h"

om::segmentationInspector::PageTools::PageTools(
    QWidget* parent, const SegmentationDataWrapper& sdw)
    : QWidget(parent), sdw_(sdw) {
  QVBoxLayout* overallContainer = new QVBoxLayout(this);
  overallContainer->addWidget(makeToolsBox());
  overallContainer->addStretch(1);
}

QGroupBox* om::segmentationInspector::PageTools::makeToolsBox() {
  QGroupBox* segmentBox = new QGroupBox("Tools");
  QGridLayout* gridSegment = new QGridLayout(segmentBox);

  AddSegmentButton* addSegmentButton = new AddSegmentButton(this);
  gridSegment->addWidget(addSegmentButton, 0, 0);
  return segmentBox;
}
