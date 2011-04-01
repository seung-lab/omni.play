#include "actions/omSelectSegmentParams.hpp"
#include "events/details/omSegmentEvent.h"

OmSegmentEvent::OmSegmentEvent(QEvent::Type type)
    : OmEvent(type, CLASS)
    , params_(boost::make_shared<OmSelectSegmentsParams>())
{}

OmSegmentEvent::OmSegmentEvent(QEvent::Type type,
                               const SegmentationDataWrapper& sdw)
    : OmEvent(type, CLASS)
    , guiParams_(new OmSegmentGUIparams())
{
    guiParams_->sdw = sdw;
}

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
        p_cast_listener->SegmentModificationEvent(this);
        return;

    case OmSegmentEvent::SEGMENT_GUI_LIST:
        p_cast_listener->SegmentGUIlistEvent(this);
        return;

    case OmSegmentEvent::SEGMENT_SELECTED:
        p_cast_listener->SegmentSelectedEvent(this);
        return;

    default:
        throw OmArgException("unknown event type");
    }
}
