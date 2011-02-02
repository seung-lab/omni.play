#ifndef OM_SEGMENT_SELECTOR_H
#define OM_SEGMENT_SELECTOR_H

#include "common/omCommon.h"
#include "utility/dataWrappers.h"

class OmSegmentation;
class OmSegmentCache;

class OmSegmentSelector {
 public:
	OmSegmentSelector(const SegmentationDataWrapper& sdw,
					  void* sender,
					  const std::string & cmt );

	void selectJustThisSegment( const OmSegID segID, const bool isSelected);
	void augmentSelectedSet( const OmSegID segID, const bool isSelected);

	void InsertSegments(const boost::unordered_set<OmSegID>& segIDs);

	void selectJustThisSegment_toggle( const OmSegID segID);
	void augmentSelectedSet_toggle( const OmSegID segID);

	bool sendEvent();
	void selectNoSegments();

	void ShouldScroll(const bool shouldScroll){
		shouldScroll_ = shouldScroll;
	}

	void AddToRecentList(const bool addToRecentList){
		addToRecentList_ = addToRecentList;
	}

	void AutoCenter(const bool autoCenter){
		autoCenter_ = autoCenter;
	}

	void AugmentListOnly(const bool augmentListOnly){
		augmentListOnly_ = augmentListOnly;
	}

private:
	SegmentDataWrapper sdw_;
	OmSegmentCache* segmentCache_;

	void* mSender;
	std::string mComment;

	const OmSegIDsSet oldSelectedIDs;
	OmSegIDsSet newSelectedIDs;

	bool mAddToRecentList;

	void setSelectedSegment(const OmSegID segID);

	bool autoCenter_;
	bool shouldScroll_;
	bool addToRecentList_;
	bool augmentListOnly_;
};

#endif
