#ifndef OM_SEGMENT_SPLIT_ACTION_H
#define OM_SEGMENT_SPLIT_ACTION_H

/*
 *
 *
 */

class OmSegmentation;
class OmSegment;


#include "system/omAction.h"

class OmSegmentSplitAction : public OmAction {

public:
	OmSegmentSplitAction(OmSegment* child, OmId segmentation);

private:
	void Action();
	void UndoAction();
	string Description();

	OmSegment* mChild;
	OmSegID mParentId;

	OmId mSegmentationId;


};

#endif
