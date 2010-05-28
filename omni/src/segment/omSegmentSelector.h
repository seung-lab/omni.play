#ifndef OM_SEGMENT_SELECTOR_H
#define OM_SEGMENT_SELECTOR_H

#include "common/omCommon.h"

class OmSegmentation;

class OmSegmentSelector
{
 public:
	OmSegmentSelector( const OmId segmentationID, void * sender, const string & cmt );
	void selectJustThisSegment( const OmSegID segID, const bool isSelected );
	void augmentSelectedSet( const OmSegID segID, const bool isSelected );
	void sendEvent();
	void selectNoSegments();
	
private:
	OmSegIDs newlySelectedSegs;
	OmSegIDs newlyUnselectedSegs;

	OmSegmentation * mSegmentation;

	void * mSender;
	string mComment;
	OmId mSegmentJustSelectedID;

};

#endif
