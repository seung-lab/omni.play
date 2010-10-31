#ifndef OM_SEGMENTATION_THRESHOLD_CHANGE_IMPL_HPP
#define OM_SEGMENTATION_THRESHOLD_CHANGE_IMPL_HPP

#include "common/omCommon.h"
#include "project/omProject.h"
#include "actions/io/omActionLoggerFS.h"
#include "segment/omSegmentCache.h"
#include "system/events/omSegmentEvent.h"
#include "utility/dataWrappers.h"
#include "volume/omSegmentation.h"

class OmSegmentationThresholdChangeActionImpl {
private:
	OmID mSegmentationId;
	float mThreshold;
	float mOldThreshold;

public:
	OmSegmentationThresholdChangeActionImpl(const OmID segmentationId,
											const float threshold)
		: mSegmentationId( segmentationId )
		, mThreshold( threshold )
	{}

	void Execute()
	{
		OmSegmentation & seg = OmProject::GetSegmentation(mSegmentationId);
		mOldThreshold = seg.GetDendThreshold();
		seg.SetDendThreshold(mThreshold);
	}

	void Undo()
	{
		OmSegmentation & seg = OmProject::GetSegmentation(mSegmentationId);
		seg.SetDendThreshold(mOldThreshold);
	}

	std::string Description() const
	{
		QString lineItem = QString("Threshold: %1").arg(mThreshold);
		return lineItem.toStdString();
	}

	QString classNameForLogFile() const {
		return "OmSegmentationThresholdChangeAction";
	}

private:
	template <typename T> friend class OmActionLoggerFSThread;

	friend class QDataStream
	&operator<<(QDataStream&, const OmSegmentationThresholdChangeActionImpl&);

	friend class QDataStream
	&operator>>(QDataStream&, OmSegmentationThresholdChangeActionImpl&);
};

#endif
