#ifndef OM_SEGMENT_EVENT_CLASS_H
#define OM_SEGMENT_EVENT_CLASS_H

/*
 *
 *	Brett Warne - bwarne@mit.edu - 3/14/09
 */


#include "system/omEvent.h"
#include "common/omCommon.h"

class OmSegmentAction;

class OmSegmentEvent : public OmEvent {

 public:
	OmSegmentEvent(QEvent::Type type);
	OmSegmentEvent(QEvent::Type type, OmId);
	OmSegmentEvent(QEvent::Type type, OmId, OmId, void*, std::string, const bool );
	void Dispatch(OmEventListener *);

	//class
	static const OmEventClass CLASS  = OM_SEGMENT_EVENT_CLASS;

	//events
	static const QEvent::Type SEGMENT_OBJECT_MODIFICATION = (QEvent::Type) (CLASS + 1);
	static const QEvent::Type SEGMENT_DATA_MODIFICATION = (QEvent::Type) (CLASS + 2);
	static const QEvent::Type SEGMENT_EDIT_SELECTION_CHANGE = (QEvent::Type) (CLASS + 3);

	OmId GetModifiedSegmentationId();
	OmId GetSegmentJustSelectedID();
	void* getSender();
	std::string getComment();
	bool getDoScroll();

 private:
	OmId  mSegmentationId;
	OmId  mSegmentJustSelectedID;
	void* mSender;
	std::string mComment;
	bool mDoScroll;
};

class OmSegmentEventListener : public OmEventListener {

public:
	OmSegmentEventListener() : OmEventListener(OmSegmentEvent::CLASS) { };

	//add/remove segment, change state, change selection
	virtual void SegmentObjectModificationEvent(OmSegmentEvent * event) = 0;

	//voxels of a segment have changed
	virtual void SegmentDataModificationEvent() = 0;

	//change segment edit selection
	virtual void SegmentEditSelectionChangeEvent() = 0;
};

#endif
