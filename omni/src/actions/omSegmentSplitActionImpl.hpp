#ifndef OM_SEGMENT_SPLIT_ACTION_IMPL_HPP
#define OM_SEGMENT_SPLIT_ACTION_IMPL_HPP

#include "common/omCommon.h"
#include "project/omProject.h"
#include "system/omEvents.h"
#include "utility/dataWrappers.h"
#include "viewGroup/omViewGroupState.h"

class OmSegmentSplitActionImpl {
private:
	OmSegmentEdge mEdge;
	OmID mSegmentationID;
	QString desc;

public:
	OmSegmentSplitActionImpl(const SegmentationDataWrapper & sdw,
							 const OmSegmentEdge & edge)
		: mEdge(edge)
		, mSegmentationID(sdw.getSegmentationID())
		, desc("Splitting: ")
	{}

	void Execute()
	{
		SegmentationDataWrapper sdw(mSegmentationID);
		mEdge = sdw.getSegmentCache()->SplitEdge(mEdge);

		desc = QString("Split seg %1 from %2")
			.arg(mEdge.childID)
			.arg(mEdge.parentID);

		std::cout << desc.toStdString() << "\n";

		OmEvents::SegmentModified();
	}

	void Undo()
	{
		SegmentationDataWrapper sdw(mSegmentationID);
		std::pair<bool, OmSegmentEdge> edge = sdw.getSegmentCache()->JoinEdge(mEdge);

		assert(edge.first && "edge could not be rejoined...");
		mEdge = edge.second;

		desc = QString("Joined seg %1 to %2")
			.arg(mEdge.childID)
			.arg(mEdge.parentID);

		OmEvents::SegmentModified();
	}

	std::string Description() const {
		return desc.toStdString();
	}

	QString classNameForLogFile() const {
		return "OmSegmentSplitAction";
	}

private:
	template <typename T> friend class OmActionLoggerFSThread;
	friend class QDataStream &operator<<(QDataStream&, const OmSegmentSplitActionImpl&);
	friend class QDataStream &operator>>(QDataStream&, OmSegmentSplitActionImpl&);
};

#endif
