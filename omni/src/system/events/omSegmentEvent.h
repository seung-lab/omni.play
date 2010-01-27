#ifndef OM_SEGMENT_EVENT_CLASS_H
#define OM_SEGMENT_EVENT_CLASS_H

/*
 *
 *	Brett Warne - bwarne@mit.edu - 3/14/09
 */

#include "volume/omVolumeTypes.h"
#include "system/omSystemTypes.h"
#include "system/omEvent.h"
#include "common/omStd.h"

#include <vmmlib/vmmlib.h>
using namespace vmml;

class OmSegmentAction;

class OmSegmentEvent:public OmEvent {

 public:
	OmSegmentEvent(QEvent::Type type);
	OmSegmentEvent(QEvent::Type type, OmId, const OmIds &);
	 OmSegmentEvent(QEvent::Type type, OmId, const OmIds &, OmId, void *user_data = NULL);
	void Dispatch(OmEventListener *);

	//class
	static const OmEventClass CLASS = OM_SEGMENT_EVENT_CLASS;

	//events
	static const QEvent::Type SEGMENT_OBJECT_MODIFICATION = (QEvent::Type) (CLASS + 1);
	static const QEvent::Type SEGMENT_DATA_MODIFICATION = (QEvent::Type) (CLASS + 2);
	static const QEvent::Type SEGMENT_EDIT_SELECTION_CHANGE = (QEvent::Type) (CLASS + 3);

	OmId GetModifiedSegmentationId();
	const OmIds & GetModifiedSegmentIds();
	const OmId GetSegmentJustSelectedID();
	void *getUserData();

 private:
	 OmId mSegmentationId;
	OmIds mSegmentIds;
	OmId mSegmentJustSelectedID;
	void *mUserData;

};

class OmSegmentEventListener:public OmEventListener {

 public:
	OmSegmentEventListener():OmEventListener(OmSegmentEvent::CLASS) {
	};

	//add/remove segment, change state, change selection
	//valid methods: GetModifiedSegmentationId(), GetModifiedSegmentIds()
	virtual void SegmentObjectModificationEvent(OmSegmentEvent * event) {
	};

	//voxels of a segment have changed
	//valid methods: GetModifiedSegmentationId(), GetModifiedSegmentIds()
	virtual void SegmentDataModificationEvent(OmSegmentEvent * event) {
	};

	//change segment edit selection
	virtual void SegmentEditSelectionChangeEvent(OmSegmentEvent * event) {
	};
};

#endif
