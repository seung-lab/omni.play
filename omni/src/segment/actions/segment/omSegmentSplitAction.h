#ifndef OM_SEGMENT_SPLIT_ACTION_H
#define OM_SEGMENT_SPLIT_ACTION_H

#include "system/omAction.h"
#include "utility/dataWrappers.h"

class OmSegmentation;
class OmSegment;

class OmSegmentSplitAction : public OmAction {

public:
	static void RunIfSplittable( OmSegment * seg1, OmSegment * seg2 );

private:
	OmSegmentSplitAction( const SegmentationDataWrapper & sdw, 
			      const OmSegmentEdge & edge );

	void Run();

	void Action();
	void UndoAction();
	string Description();
	void save(const string & comment);

	OmSegmentEdge mEdge;
	SegmentationDataWrapper m_sdw;

	QString desc;
};

#endif
