#ifndef OM_SEGMENT_SPLIT_ACTION_H
#define OM_SEGMENT_SPLIT_ACTION_H

#include "system/omAction.h"
#include "segment/omSegmentEdge.h"

class SegmentDataWrapper;
class SegmentationDataWrapper;
class OmSegmentation;
class OmSegment;
class OmViewGroupState;
class OmSegmentSplitActionImpl;

class OmSegmentSplitAction : public OmAction {

public:
	static void DoFindAndSplitSegment(const SegmentDataWrapper& sdw,
					  OmViewGroupState* vgs);

private:
	OmSegmentSplitAction( const SegmentationDataWrapper & sdw,
			      const OmSegmentEdge & edge );

	void Run();

	void Action();
	void UndoAction();
	std::string Description();
	void save(const std::string& comment);

	static void RunIfSplittable( OmSegment * seg1, OmSegment * seg2 );

	boost::shared_ptr<OmSegmentSplitActionImpl> impl_;
};

#endif
