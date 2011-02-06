#ifndef UPDATE_SEGMENT_PROPERTIES_DIALOG_HPP
#define UPDATE_SEGMENT_PROPERTIES_DIALOG_HPP

#include "actions/omSelectSegmentParams.hpp"
#include "events/omSegmentEvent.h"
#include "gui/myInspectorWidget.h"
#include "zi/omUtility.h"
#include "gui/inspectors/segObjectInspector.h"

class UpdateSegmentPropertiesDialog
    : public OmSegmentEventListener,
      private om::singletonBase<UpdateSegmentPropertiesDialog>{
public:
    static void SetInspectorProperties(InspectorProperties* ip){
        instance().ip_ = ip;
    }

private:
    InspectorProperties* ip_;

    UpdateSegmentPropertiesDialog()
        : ip_(NULL)
    {}

    void SegmentObjectModificationEvent(OmSegmentEvent* event)
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

    void SegmentDataModificationEvent()
    {}

    void SegmentEditSelectionChangeEvent()
    {}

    void VoxelSelectionModificationEvent()
    {}

    friend class zi::singleton<UpdateSegmentPropertiesDialog>;
};

#endif
