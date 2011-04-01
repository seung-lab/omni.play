#ifndef UPDATE_SEGMENT_PROPERTIES_DIALOG_IMPL_HPP
#define UPDATE_SEGMENT_PROPERTIES_DIALOG_IMPL_HPP

#include "actions/omSelectSegmentParams.hpp"
#include "events/details/omSegmentEvent.h"
#include "gui/mainWindow/inspectorWidget.h"
#include "gui/inspectors/segObjectInspector.h"
#include "gui/inspectors/inspectorProperties.h"

class UpdateSegmentPropertiesDialogImpl : public OmSegmentEventListener {
private:
    InspectorProperties *const ip_;

public:
    UpdateSegmentPropertiesDialogImpl(InspectorProperties* ip)
        : ip_(ip)
    {}

    ~UpdateSegmentPropertiesDialogImpl()
    {}

    void SegmentModificationEvent(OmSegmentEvent* event)
    {
        if(!ip_ || !ip_->WidgetIsViewable()){
            return;
        }

        const SegmentDataWrapper& sdw = event->Params().sdw;

        if(!sdw.IsSegmentationValid()){
            return;
        }

        ip_->setOrReplaceWidget( new SegObjectInspector(sdw, ip_),
                                 QString("Segmentation %1: Segment %2")
                                 .arg(sdw.GetSegmentationID())
                                 .arg(sdw.GetSegmentID()));
    }
};

#endif
