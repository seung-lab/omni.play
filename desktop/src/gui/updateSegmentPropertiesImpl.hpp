#pragma once

#include "actions/omSelectSegmentParams.hpp"
#include "events/details/omSegmentEvent.h"
#include "gui/mainWindow/inspectorWidget.h"
#include "gui/inspectors/segmentInspector.h"
#include "gui/inspectors/inspectorProperties.h"

class UpdateSegmentPropertiesDialogImpl : public om::event::SegmentEventListener {
 private:
  InspectorProperties* const ip_;

 public:
  UpdateSegmentPropertiesDialogImpl(InspectorProperties* ip) : ip_(ip) {}

  ~UpdateSegmentPropertiesDialogImpl() {}

  void SegmentGUIlistEvent(om::event::SegmentEvent*) {}

  void SegmentSelectedEvent(om::event::SegmentEvent*) {}

  void SegmentModificationEvent(om::event::SegmentEvent* event) {
    if (!ip_ || !ip_->WidgetIsViewable()) {
      return;
    }

    const SegmentDataWrapper& sdw = event->Params().sdw;

    if (!sdw.IsSegmentationValid()) {
      return;
    }

    ip_->setOrReplaceWidget(
        new SegmentInspector(sdw, ip_),
        QString("Segmentation %1: Segment %2").arg(sdw.GetSegmentationID())
            .arg(sdw.GetSegmentID()));
  }
};
