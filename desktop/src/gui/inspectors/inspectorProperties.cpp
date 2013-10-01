#include "actions/omSelectSegmentParams.hpp"
#include "events/details/uiEvent.h"
#include "events/details/segmentEvent.h"
#include "gui/inspectors/inspectorProperties.h"
#include "gui/inspectors/segmentInspector.h"
#include "viewGroup/omViewGroupState.h"

InspectorProperties::InspectorProperties(QWidget* parent, OmViewGroupState* vgs)
    : QDialog(parent), vgs_(vgs) {
  mainLayout_ = new QVBoxLayout();
  setLayout(mainLayout_);
}

InspectorProperties::~InspectorProperties() {}

void InspectorProperties::CloseDialog() { QDialog::done(0); }

void InspectorProperties::SetOrReplaceWidget(QWidget* newWidget,
                                             const QString& title) {
  if (widget_) {
    mainLayout_->removeWidget(widget_.get());
    widget_->close();
  }

  widget_.reset(newWidget);

  mainLayout_->addWidget(newWidget);

  setWindowTitle(title);

  if (!isVisible()) {
    show();
    raise();
    activateWindow();
  }
}

void InspectorProperties::UpdateSegmentPropWidgetEvent(
    om::event::UIEvent* event) {
  SetOrReplaceWidget(event->Widget(), event->Title());
}

void InspectorProperties::SegmentModificationEvent(
    om::event::SegmentEvent* event) {
  if (!isVisible()) {
    return;
  }

  const SegmentDataWrapper& sdw = event->Params().sdw;

  if (!sdw.IsValidWrapper()) {
    return;
  }

  SetOrReplaceWidget(new SegmentInspector(sdw, this),
                     QString("Segmentation %1: Segment %2")
                         .arg(sdw.GetSegmentationID()).arg(sdw.GetSegmentID()));
}
