#ifndef OM_SEGMENT_SELECTOR_H
#define OM_SEGMENT_SELECTOR_H

#include "common/omCommon.h"

class OmSegmentation;

class OmSegmentSelector
{
 public:
	OmSegmentSelector( const OmId segmentationID, void * sender,
					   const std::string & cmt );
	void selectJustThisSegment( const OmSegID segID, const bool isSelected );
	void augmentSelectedSet( const OmSegID segID, const bool isSelected );

	void selectJustThisSegment_toggle( const OmSegID segID );
	void augmentSelectedSet_toggle( const OmSegID segID );

	bool sendEvent();
	void selectNoSegments();

	void setAddToRecentList(const bool shouldAdd);

private:
	OmSegmentation * mSegmentation;

	OmId mSegmentJustSelectedID;
	void * mSender;
	std::string mComment;

	const OmSegIDsSet oldSelectedIDs;
	OmSegIDsSet newSelectedIDs;

	bool mAddToRecentList;

	void setEditSelection(const OmSegID segID);
};

#endif
