#ifndef OM_SEGMENT_JOIN_ACTION_IMPL_HPP
#define OM_SEGMENT_JOIN_ACTION_IMPL_HPP

#include "common/omCommon.h"
#include "actions/io/omActionLoggerFS.h"
#include "project/omProject.h"
#include "segment/omSegmentCache.h"
#include "utility/dataWrappers.h"
#include "volume/omSegmentation.h"
#include "volume/omVolume.h"

class OmSegmentJoinActionImpl {
public:
	OmSegmentJoinActionImpl(const OmID segmentationId,
							const OmSegIDsSet& selectedSegmentIds)
		: mSegmentationId( segmentationId )
		, mSelectedSegmentIds( selectedSegmentIds )
	{}

	void Execute()
	{
		OmSegmentation & seg = OmProject::GetSegmentation(mSegmentationId);
		seg.GetSegmentCache()->JoinTheseSegments(mSelectedSegmentIds);
	}

	void Undo()
	{
		OmSegmentation & seg = OmProject::GetSegmentation(mSegmentationId);
		seg.GetSegmentCache()->UnJoinTheseSegments(mSelectedSegmentIds);
	}

	std::string Description()
	{
		QString lineItem = QString("Joined: ");
		foreach( const OmID segId, mSelectedSegmentIds){
			lineItem += QString("seg %1 + ").arg(segId);
		}

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
