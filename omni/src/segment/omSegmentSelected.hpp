#ifndef OM_SEGMENT_EDITOR_H
#define OM_SEGMENT_EDITOR_H

#include "actions/omActions.hpp"
#include "common/omCommon.h"
#include "system/cache/omCacheManager.h"
#include "system/omEvents.h"
#include "utility/dataWrappers.h"
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

	static void RandomizeColor()
	{
		if(!instance().sdw_.isValidWrapper()){
			return;
		}
		instance().sdw_.RandomizeColor();
		OmCacheManager::TouchFresheness();
		OmEvents::Redraw2d();
	}

	static void ToggleValid()
	{
		if(!instance().sdw_.isValidWrapper()){
			return;
		}

		OmSegment* seg = instance().sdw_.getSegment();

		if(seg->IsValid()){
			OmActions::ValidateSegment(instance().sdw_, om::SET_NOT_VALID);
		} else {
			OmActions::ValidateSegment(instance().sdw_, om::SET_VALID);
		}

		OmEvents::SegmentModified();
	}

private:
	OmSegmentSelected(){}
	~OmSegmentSelected(){}

	SegmentDataWrapper sdw_;

	friend class zi::singleton<OmSegmentSelected>;
};

#endif
