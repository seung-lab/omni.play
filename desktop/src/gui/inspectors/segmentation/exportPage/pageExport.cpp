#include "gui/inspectors/segmentation/exportPage/buttons/exportButton.hpp"
#include "gui/inspectors/segmentation/exportPage/buttons/exportButtonRaw.hpp"
#include "gui/inspectors/segmentation/exportPage/buttons/exportDescendantList.hpp"
#include "gui/inspectors/segmentation/exportPage/buttons/exportMST.hpp"
#include "gui/inspectors/segmentation/exportPage/buttons/exportSegmentList.hpp"
#include "gui/inspectors/segmentation/exportPage/buttons/segmentationInspectorButtons.hpp"
#include "gui/inspectors/segmentation/exportPage/pageExport.h"
#include "utility/omStringHelpers.h"

om::segmentationInspector::PageExport::PageExport(
    QWidget* parent, const SegmentationDataWrapper& sdw)
    : QWidget(parent), sdw_(sdw) {
  QVBoxLayout* overallContainer = new QVBoxLayout(this);
  overallContainer->addWidget(makeActionsBox());
}

QGroupBox* om::segmentationInspector::PageExport::makeActionsBox() {
  QGroupBox* actionsBox = new QGroupBox("Actions");
  QGridLayout* gridAction = new QGridLayout(actionsBox);

  ExportButton* exportButton = new ExportButton(this);
  gridAction->addWidget(exportButton, 2, 0, 1, 1);

  ExportButtonRaw* exportButtonRaw = new ExportButtonRaw(this);
  gridAction->addWidget(exportButtonRaw, 2, 1, 1, 1);

  ExportSegmentList* segList = new ExportSegmentList(this);
  gridAction->addWidget(segList, 3, 0, 1, 1);

  ExportMST* mstList = new ExportMST(this);
  gridAction->addWidget(mstList, 3, 1, 1, 1);

  RebuildCenterOfSegmentDataButton* rebuildSegmentButton =
      new RebuildCenterOfSegmentDataButton(this);
  gridAction->addWidget(rebuildSegmentButton, 5, 1, 1, 1);

  ExportDescendantList* exportSegmentList = new ExportDescendantList(this);
  gridAction->addWidget(exportSegmentList, 6, 0, 1, 1);

  return actionsBox;
}
