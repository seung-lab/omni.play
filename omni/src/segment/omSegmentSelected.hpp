#ifndef OM_SEGMENT_EDITOR_H
#define OM_SEGMENT_EDITOR_H

#include "common/omCommon.h"
#include "utility/dataWrappers.h"
#include "system/omEvents.h"
#include "zi/omUtility.h"

class OmSegmentSelected : private om::singletonBase<OmSegmentSelected> {
public:
	static void Delete(){
		instance().sdw_ = SegmentDataWrapper();
	}

	static void Set(const OmID segmentationID, const OmSegID segmentID)
	{
		Set(SegmentDataWrapper(segmentationID, segmentID));
	}

	static void Set(const SegmentDataWrapper& sdw)
	{
		instance().sdw_ = sdw;
		OmEvents::SegmentEditSelectionChanged();
	}

	static SegmentDataWrapper Get() {
		return instance().sdw_;
	}

private:
	OmSegmentSelected(){}
	~OmSegmentSelected(){}

	SegmentDataWrapper sdw_;

	friend class zi::singleton<OmSegmentSelected>;
};

#endif
