#ifndef OM_SEGMENT_EDITOR_H
#define OM_SEGMENT_EDITOR_H

#include "common/omCommon.h"
#include "utility/dataWrappers.h"
#include "system/omEvents.h"

#include <zi/utility>

class OmSegmentEditor : boost::noncopyable {
public:
	static void Delete(){
		Instance().mEditSegmentation = 0;
		Instance().mEditSegment = 0;
	}

	static void SetEditSelection(const OmId segmentation,
				     const OmSegID segment){
		Instance().mEditSegmentation = segmentation;
		Instance().mEditSegment = segment;
		OmEvents::SegmentEditSelectionChanged();
	}

	static SegmentDataWrapper GetEditSelection(){
		return SegmentDataWrapper(Instance().mEditSegmentation,
					  Instance().mEditSegment);
	}

private:
	OmSegmentEditor()
		: mEditSegmentation(0)
		, mEditSegment(0) {}
	~OmSegmentEditor(){}
	static inline OmSegmentEditor& Instance(){
		return zi::Singleton<OmSegmentEditor>::Instance();
	}

	OmId mEditSegmentation;
	OmSegID mEditSegment;

	friend class zi::Singleton<OmSegmentEditor>;
};

#endif
