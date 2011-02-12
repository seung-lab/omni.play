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

	void selectJustThisSegment( const OmSegID segID, const bool isSelected, const bool center=false );
	void augmentSelectedSet( const OmSegID segID, const bool isSelected, const bool center=false );

	void selectJustThisSegment_toggle( const OmSegID segID, const bool center=false );
	void augmentSelectedSet_toggle( const OmSegID segID, const bool center=false );

	bool sendEvent();
	void selectNoSegments();

	void setAddToRecentList(const bool shouldAdd);

private:
	SegmentDataWrapper sdw_;
	OmSegmentCache* segmentCache_;

	void * mSender;
	std::string mComment;

	const OmSegIDsSet oldSelectedIDs;
	OmSegIDsSet newSelectedIDs;

	bool mAddToRecentList;

	void setSelectedSegment(const OmSegID segID);

	bool mAutoCenter;
};

#endif
