#include "actions/omSelectSegmentParams.hpp"
#include "system/events/omSegmentEvent.h"

OmSegmentEvent::OmSegmentEvent(QEvent::Type type)
    : OmEvent(type, CLASS)
    , params_(boost::make_shared<OmSelectSegmentsParams>())
{}

OmSegmentEvent::OmSegmentEvent(QEvent::Type type,
                               boost::shared_ptr<OmSelectSegmentsParams> params)
    : OmEvent(type, CLASS)
    , params_(params)
{}

/*
 *  Dispatch event based on event type.
 */

void OmSegmentEvent::Dispatch(OmEventListener * pListener)
{
    //cast to proper listener
    OmSegmentEventListener *p_cast_listener =
        dynamic_cast < OmSegmentEventListener * >(pListener);

    assert(p_cast_listener);

    switch (type()) {
    case OmSegmentEvent::SEGMENT_OBJECT_MODIFICATION:
        p_cast_listener->SegmentObjectModificationEvent(this);
        return;
    case OmSegmentEvent::SEGMENT_DATA_MODIFICATION:
        p_cast_listener->SegmentDataModificationEvent();
        return;
    case OmSegmentEvent::SEGMENT_EDIT_SELECTION_CHANGE:
        p_cast_listener->SegmentEditSelectionChangeEvent();
        return;
    default:
        throw OmArgException("unknown event type");
    }
}
