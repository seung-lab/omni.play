#include "omSegmentEvent.h"

OmSegmentEvent::OmSegmentEvent(QEvent::Type type)
	: OmEvent(type, CLASS)
	, mSegmentationId(0)
	, mSegmentJustSelectedID(0)
	, mSender(NULL)
	, mComment("unknown")
	, mDoScroll(true)
{
}

OmSegmentEvent::OmSegmentEvent(QEvent::Type type, OmID segmentationId)
	: OmEvent(type, CLASS)
	, mSegmentationId(segmentationId)
	, mSegmentJustSelectedID(0)
	, mSender(NULL)
	, mComment("unknown")
	, mDoScroll(true)
{
}

OmSegmentEvent::OmSegmentEvent(QEvent::Type type,
			       OmID segmentationId,
			       const OmID segmentJustSelectedID,
			       void* sender,
			       std::string comment,
			       const bool doScroll )
	: OmEvent(type, CLASS)
	, mSegmentationId( segmentationId )
	, mSegmentJustSelectedID( segmentJustSelectedID )
	, mSender( sender )
	, mComment( comment )
	, mDoScroll(doScroll)
{
}

OmID OmSegmentEvent::GetModifiedSegmentationId()
{
	return mSegmentationId;
}

OmID OmSegmentEvent::GetSegmentJustSelectedID()
{
	return mSegmentJustSelectedID;
}

void* OmSegmentEvent::getSender()
{
	return mSender;
}

std::string OmSegmentEvent::getComment()
{
	return mComment;
}

bool OmSegmentEvent::getDoScroll()
{
	return mDoScroll;
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
