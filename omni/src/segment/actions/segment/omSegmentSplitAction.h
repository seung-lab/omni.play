#ifndef OM_SEGMENT_SPLIT_ACTION_H
#define OM_SEGMENT_SPLIT_ACTION_H

/*
 *
 *
 */

class OmSegmentation;
class OmSegment;
class OmSegmentEdge;

#include "system/omAction.h"

class OmSegmentSplitAction : public OmAction {

public:
	OmSegmentSplitAction(OmSegment * seg1, OmSegment * seg2 );

private:
	void Action();
	void UndoAction();
	string Description();

	OmSegment * mSeg1;
	OmSegment * mSeg2;

	OmSegmentEdge * edge;

	QString desc;
};

#endif
