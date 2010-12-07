#include "omSegmentEvent.h"

OmSegmentEvent::OmSegmentEvent(QEvent::Type type)
	: OmEvent(type, CLASS)
	, mSender(NULL)
	, mComment("unknown")
	, mDoScroll(true)
{
}

OmSegmentEvent::OmSegmentEvent(QEvent::Type type,
			       const SegmentDataWrapper& sdw,
			       void* sender,
			       std::string comment,
			       const bool doScroll )
	: OmEvent(type, CLASS)
	, sdw_(sdw)
	, mSender( sender )
	, mComment( comment )
	, mDoScroll(doScroll)
{
}

/*
 *	Dispatch event based on event type.
 */

void OmSegmentEvent::Dispatch(OmEventListener * pListener)
{

	//cast to proper listener
	OmSegmentEventListener *p_cast_listener = dynamic_cast < OmSegmentEventListener * >(pListener);
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
		assert(false);
	}

}
