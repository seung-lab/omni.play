#ifndef OM_SEGMENT_GROUP_ACTION_IMPL_HPP
#define OM_SEGMENT_GROUP_ACTION_IMPL_HPP

#include "project/omSegmentationManager.h"
#include "common/omCommon.h"
#include "project/omProject.h"
#include "project/omProjectVolumes.h"
#include "segment/omSegmentCache.h"
#include "system/omGroups.h"
#include "volume/omSegmentation.h"

class OmSegmentGroupActionImpl {
private:

	OmID mSegmentationId;
	OmGroupName mName;
	bool mCreate;
	OmSegIDsSet mSelectedSegmentIds;

public:
	OmSegmentGroupActionImpl() {}
	OmSegmentGroupActionImpl( const OmID segmentationId,
							  const OmSegIDsSet& selectedSegmentIds,
							  const OmGroupName name,
							  const bool create)
		: mSegmentationId( segmentationId )
		, mName(name)
		, mCreate(create)
		, mSelectedSegmentIds( selectedSegmentIds )
	{}

	void Execute()
	{
		OmSegmentation & seg = OmProject::Volumes().Segmentations().GetSegmentation(mSegmentationId);
		if(mCreate) {
        	seg.Groups()->SetGroup(mSelectedSegmentIds, mName);
		} else {
        	seg.Groups()->UnsetGroup(mSelectedSegmentIds, mName);
		}

	}

	void Undo()
	{
		OmSegmentation & seg = OmProject::Volumes().Segmentations().GetSegmentation(mSegmentationId);
		if(!mCreate) {
        	seg.Groups()->SetGroup(mSelectedSegmentIds, mName);
		} else {
        	seg.Groups()->UnsetGroup(mSelectedSegmentIds, mName);
		}
	}

	std::string Description() const
	{
		QString lineItem = QString("Grouped: ");
		foreach( const OmID segId, mSelectedSegmentIds){
			lineItem += QString("seg %1 + ").arg(segId);
		}

		return lineItem.toStdString();
	}

	QString classNameForLogFile() const {
		return "OmSegmentGroupAction";
	}

private:
	template <typename T> friend class OmActionLoggerThread;
	friend class QDataStream &operator<<(QDataStream&, const OmSegmentGroupActionImpl&);
	friend class QDataStream &operator>>(QDataStream&,  OmSegmentGroupActionImpl&);
};

#endif
