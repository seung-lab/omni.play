
#include "omSegmentEvent.h"

#define DEBUG 0

OmSegmentEvent::OmSegmentEvent(QEvent::Type type)
 : OmEvent(type, CLASS)
{
	mSegmentJustSelectedID = -1;
	mSender = NULL;
	mComment = "unknown";
}

OmSegmentEvent::OmSegmentEvent(QEvent::Type type, OmId segmentationId, const OmIds & segIds)
:OmEvent(type, CLASS)
{
	mSegmentationId = segmentationId;
	mSegmentIds = segIds;
	mSegmentJustSelectedID = -1;
	mSender = NULL;
	mComment = "unknown";
}

OmSegmentEvent::OmSegmentEvent(QEvent::Type type,
			       OmId segmentationId,
			       const OmIds & segIds, 
			       const OmId segmentJustSelectedID, 
			       void* sender, 
			       string comment)
:OmEvent(type, CLASS)
{
	mSegmentationId = segmentationId;
	mSegmentIds = segIds;
	mSegmentJustSelectedID = segmentJustSelectedID;
	mSender = sender;
	mComment = comment;
}

OmId OmSegmentEvent::GetModifiedSegmentationId()
{
	return mSegmentationId;
}

const OmIds & OmSegmentEvent::GetModifiedSegmentIds()
{
	return mSegmentIds;
}

const OmId OmSegmentEvent::GetSegmentJustSelectedID()
{
	return mSegmentJustSelectedID;
}

void* OmSegmentEvent::getSender()
{
	return mSender;
}

string OmSegmentEvent::getComment()
{
	return mComment;
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
		p_cast_listener->SegmentDataModificationEvent(this);
		return;

	case OmSegmentEvent::SEGMENT_EDIT_SELECTION_CHANGE:
		p_cast_listener->SegmentEditSelectionChangeEvent(this);
		return;

	default:
		assert(false);
	}

}
