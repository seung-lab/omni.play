#ifndef OM_SEGMENT_EDITOR_H
#define OM_SEGMENT_EDITOR_H

#include "common/omCommon.h"
#include "utility/dataWrappers.h"
#include "system/omEvents.h"
#include "zi/omUtility.h"

class OmSegmentEditor : private om::singletonBase<OmSegmentEditor> {
public:
	static void Delete(){
		instance().sdw_ = SegmentDataWrapper();
	}

	static void SetEditSelection(const OmID segmentationID,
								 const OmSegID segmentID)
	{
		instance().sdw_ = SegmentDataWrapper(segmentationID, segmentID);
		OmEvents::SegmentEditSelectionChanged();
	}

	static SegmentDataWrapper GetEditSelection() {
		return instance().sdw_;
	}

private:
	OmSegmentEditor(){}
	~OmSegmentEditor(){}

	SegmentDataWrapper sdw_;

	friend class zi::singleton<OmSegmentEditor>;
};

#endif
