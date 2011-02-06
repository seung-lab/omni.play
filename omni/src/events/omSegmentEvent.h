#ifndef OM_SEGMENT_EVENT_CLASS_H
#define OM_SEGMENT_EVENT_CLASS_H

/*
 *
 * Brett Warne - bwarne@mit.edu - 3/14/09
 */

#include "events/omEvent.h"
#include "common/omCommon.h"

class OmSegmentAction;
class OmSelectSegmentsParams;

class OmSegmentEvent : public OmEvent {

public:
    OmSegmentEvent(QEvent::Type type);
    OmSegmentEvent(QEvent::Type type, boost::shared_ptr<OmSelectSegmentsParams> params);

    void Dispatch(OmEventListener *);

//class
    static const OmEventClass CLASS  = OM_SEGMENT_EVENT_CLASS;

//events
    static const QEvent::Type SEGMENT_OBJECT_MODIFICATION = (QEvent::Type) (CLASS + 1);
    static const QEvent::Type SEGMENT_DATA_MODIFICATION = (QEvent::Type) (CLASS + 2);
    static const QEvent::Type SEGMENT_EDIT_SELECTION_CHANGE = (QEvent::Type) (CLASS + 3);

    inline const OmSelectSegmentsParams& Params(){
        return *params_;
    }

private:
    boost::shared_ptr<OmSelectSegmentsParams> params_;
};

class OmSegmentEventListener : public OmEventListener {

public:
    OmSegmentEventListener()
        : OmEventListener(OmSegmentEvent::CLASS)
    {}

    //add/remove segment, change state, change selection
    virtual void SegmentObjectModificationEvent(OmSegmentEvent * event) = 0;

    //voxels of a segment have changed
    virtual void SegmentDataModificationEvent() = 0;

    //change segment edit selection
    virtual void SegmentEditSelectionChangeEvent() = 0;
};

#endif
