#ifndef OM_SEGMENTATION_THRESHOLD_CHANGE_IMPL_HPP
#define OM_SEGMENTATION_THRESHOLD_CHANGE_IMPL_HPP

#include "project/omSegmentationManager.h"
#include "actions/io/omActionLogger.hpp"
#include "common/omCommon.h"
#include "project/omProject.h"
#include "project/omProjectVolumes.h"
#include "segment/omSegmentCache.h"
#include "system/events/omSegmentEvent.h"
#include "system/omEvents.h"
#include "utility/dataWrappers.h"
#include "volume/omSegmentation.h"

class OmSegmentationThresholdChangeActionImpl {
private:
	OmID mSegmentationId;
	float mThreshold;
	float mOldThreshold;

public:
	OmSegmentationThresholdChangeActionImpl()
	{}

	OmSegmentationThresholdChangeActionImpl(const OmID segmentationId,
											const float threshold)
		: mSegmentationId( segmentationId )
		, mThreshold( threshold )
	{}

	void Execute()
	{
		OmSegmentation & seg = OmProject::Volumes().Segmentations().GetSegmentation(mSegmentationId);
		mOldThreshold = seg.GetDendThreshold();
		seg.SetDendThreshold(mThreshold);
		OmEvents::SegmentModified();
	}

	void Undo()
	{
		OmSegmentation & seg = OmProject::Volumes().Segmentations().GetSegmentation(mSegmentationId);
		seg.SetDendThreshold(mOldThreshold);
		OmEvents::SegmentModified();
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
	template <typename T> friend class OmActionLoggerThread;

	friend class QDataStream
	&operator<<(QDataStream&, const OmSegmentationThresholdChangeActionImpl&);

	friend class QDataStream
	&operator>>(QDataStream&, OmSegmentationThresholdChangeActionImpl&);
};

#endif
