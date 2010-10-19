#ifndef OM_SEGMENT_SPLIT_ACTION_H
#define OM_SEGMENT_SPLIT_ACTION_H

#include "system/omAction.h"
#include "segment/omSegmentEdge.h"

class SegmentDataWrapper;
class SegmentationDataWrapper;
class OmSegmentation;
class OmSegment;
class OmViewGroupState;

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
	QString classNameForLogFile(){return "OmSegmentSplitAction";}

	OmSegmentEdge mEdge;
	OmID mSegmentationID;

	QString desc;

	static void RunIfSplittable( OmSegment * seg1, OmSegment * seg2 );

	template <typename T> friend class OmActionLoggerFSThread;
	friend class QDataStream &operator<<(QDataStream & out, const OmSegmentSplitAction & a );
	friend class QDataStream &operator>>(QDataStream & in,  OmSegmentSplitAction & a );
};

#endif
