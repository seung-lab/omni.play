#include "gui/inspectors/segmentation/statsPage/pageStats.h"
#include "utility/omStringHelpers.h"

om::segmentationInspector::PageStats::PageStats(
    QWidget* parent, const SegmentationDataWrapper& sdw)
    : QWidget(parent), sdw_(sdw) {
  QVBoxLayout* overallContainer = new QVBoxLayout(this);
  overallContainer->addWidget(makeStatsBox());
  overallContainer->addStretch(1);
}

QGroupBox* om::segmentationInspector::PageStats::makeStatsBox() {
  QGroupBox* statsBox = new QGroupBox("Stats");
  QGridLayout* grid = new QGridLayout(statsBox);

  QLabel* labelNumSegments = new QLabel(statsBox);
  labelNumSegments->setText("number of segments:");
  grid->addWidget(labelNumSegments, 0, 0);
  QLabel* labelNumSegmentsNum = new QLabel(statsBox);

  QString commaNumSegs =
      OmStringHelpers::HumanizeNumQT(sdw_.getNumberOfSegments());
  labelNumSegmentsNum->setText(commaNumSegs);
  grid->addWidget(labelNumSegmentsNum, 0, 1);

  QLabel* labelNumTopSegments = new QLabel(statsBox);
  labelNumTopSegments->setText("number of top-level segments:");
  grid->addWidget(labelNumTopSegments, 1, 0);
  QLabel* labelNumTopSegmentsNum = new QLabel(statsBox);

  QString commaNumTopSegs =
      OmStringHelpers::HumanizeNumQT(sdw_.getNumberOfTopSegments());
  labelNumTopSegmentsNum->setText(commaNumTopSegs);
  grid->addWidget(labelNumTopSegmentsNum, 1, 1);

  return statsBox;
}