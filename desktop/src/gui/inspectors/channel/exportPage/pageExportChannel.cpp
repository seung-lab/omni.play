#include "gui/inspectors/channel/exportPage/buttons/exportButtonChannel.hpp"
#include "gui/inspectors/channel/exportPage/buttons/channelInspectorButtons.hpp"
#include "gui/inspectors/channel/exportPage/pageExportChannel.h"
#include "utility/omStringHelpers.h"

om::channelInspector::PageExport::PageExport(QWidget* parent,
                                             const ChannelDataWrapper& cdw)
    : QWidget(parent), cdw_(cdw) {
  QVBoxLayout* overallContainer = new QVBoxLayout(this);
  overallContainer->addWidget(makeExportBox());
}

QGroupBox* om::channelInspector::PageExport::makeExportBox() {
  QGroupBox* actionsBox = new QGroupBox("Actions");
  QGridLayout* gridAction = new QGridLayout(actionsBox);

  ExportButton* exportButton = new ExportButton(this, GetCDW());
  gridAction->addWidget(exportButton, 2, 0, 1, 1);

  return actionsBox;
}
