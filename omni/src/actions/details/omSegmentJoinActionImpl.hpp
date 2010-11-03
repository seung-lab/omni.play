#ifndef OM_SEGMENT_JOIN_ACTION_IMPL_HPP
#define OM_SEGMENT_JOIN_ACTION_IMPL_HPP

#include "common/omCommon.h"
#include "actions/io/omActionLoggerFS.h"
#include "segment/omSegmentCache.h"
#include "utility/dataWrappers.h"
#include "volume/omSegmentation.h"
#include "volume/omVolume.h"

class OmSegmentJoinActionImpl {
public:
	OmSegmentJoinActionImpl(const OmID segmentationId,
							const OmSegIDsSet& selectedSegmentIds)
		: mSegmentationId(segmentationId)
		, mSelectedSegmentIds(selectedSegmentIds)
	{}

	void Execute()
	{
		SegmentationDataWrapper sdw(mSegmentationId);
		mSelectedSegmentIds =
			sdw.getSegmentCache()->JoinTheseSegments(mSelectedSegmentIds);
	}

	void Undo()
	{
		SegmentationDataWrapper sdw(mSegmentationId);
		mSelectedSegmentIds =
			sdw.getSegmentCache()->UnJoinTheseSegments(mSelectedSegmentIds);
	}

	std::string Description()
	{
		if(!mSelectedSegmentIds.size()){
			return "did not join segments";
		}

		QStringList segs;
		foreach(const OmSegID& segID, mSelectedSegmentIds){
			segs << QString::number(segID);
		}

		const QString lineItem = "Joined segments: " + segs.join(", ");
		return lineItem.toStdString();
	}

	QString classNameForLogFile() const {
		return "OmSegmentJoinAction";
	}

private:
	OmID mSegmentationId;
	OmSegIDsSet mSelectedSegmentIds;

	template <typename T> friend class OmActionLoggerFSThread;
	friend class QDataStream &operator<<(QDataStream&, const OmSegmentJoinActionImpl&);
	friend class QDataStream &operator>>(QDataStream&, OmSegmentJoinActionImpl&);


};

#endif
