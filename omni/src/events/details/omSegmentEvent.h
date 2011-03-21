#ifndef OM_SEGMENT_EVENT_CLASS_H
#define OM_SEGMENT_EVENT_CLASS_H

/*
 *
 * Brett Warne - bwarne@mit.edu - 3/14/09
 */

#include "events/details/omEvent.h"
#include "common/omCommon.h"

class OmSegmentAction;
class OmSelectSegmentsParams;
class SegmentationDataWrapper;
class OmSegmentGUIparams;

class OmSegmentEvent : public OmEvent {

public:
    OmSegmentEvent(QEvent::Type type);
    OmSegmentEvent(QEvent::Type type, const SegmentationDataWrapper& sdw);
    OmSegmentEvent(QEvent::Type type, boost::shared_ptr<OmSelectSegmentsParams> params);

    void Dispatch(OmEventListener *);

//class
    static const OmEventClass CLASS  = OM_SEGMENT_EVENT_CLASS;

//events
    static const QEvent::Type SEGMENT_OBJECT_MODIFICATION = (QEvent::Type) (CLASS + 1);
    static const QEvent::Type SEGMENT_GUI_LIST = (QEvent::Type) (CLASS + 2);

    inline const OmSelectSegmentsParams& Params(){
        return *params_;
    }

    inline const OmSegmentGUIparams& GUIparams(){
        return *guiParams_;
    }

private:
    boost::shared_ptr<OmSelectSegmentsParams> params_;
    boost::scoped_ptr<OmSegmentGUIparams> guiParams_;
};

class OmSegmentEventListener : public OmEventListener {

public:
    OmSegmentEventListener()
        : OmEventListener(OmSegmentEvent::CLASS)
    {}

    //add/remove segment, change state, change selection
    virtual void SegmentModificationEvent(OmSegmentEvent* event) = 0;

    virtual void SegmentGUIlistEvent(OmSegmentEvent*)
    {}
};

#endif
