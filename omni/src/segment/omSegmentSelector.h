#ifndef OM_SEGMENT_SELECTOR_H
#define OM_SEGMENT_SELECTOR_H

#include "common/omCommon.h"

class OmSegmentation;
class OmSegmentCache;

class OmSegmentSelector
{
 public:
	OmSegmentSelector(OmSegmentation* segmentation,
					  void* sender,
					  const std::string& cmt);
	OmSegmentSelector( const OmID segmentationID,
					   void* sender,
					   const std::string & cmt );

	void selectJustThisSegment( const OmSegID segID, const bool isSelected );
	void augmentSelectedSet( const OmSegID segID, const bool isSelected );

	void selectJustThisSegment_toggle( const OmSegID segID );
	void augmentSelectedSet_toggle( const OmSegID segID );

	bool sendEvent();
	void selectNoSegments();

	void setAddToRecentList(const bool shouldAdd);

private:
	OmSegmentation *const mSegmentation;
	OmSegmentCache *const segmentCache_;

	OmID mSegmentJustSelectedID;
	void * mSender;
	std::string mComment;

	const OmSegIDsSet oldSelectedIDs;
	OmSegIDsSet newSelectedIDs;

	bool mAddToRecentList;

	void setSelectedSegment(const OmSegID segID);
};

#endif
