#ifndef OM_SEGMENT_UNCERTAIN_ACTION_IMPL_HPP
#define OM_SEGMENT_UNCERTAIN_ACTION_IMPL_HPP

#include "common/omCommon.h"
#include "project/omProject.h"
#include "system/cache/omCacheManager.h"
#include "segment/omSegmentUncertain.hpp"

class OmSegmentUncertainActionImpl{
private:
	OmID mSegmentationId;
	bool uncertain_;
	boost::shared_ptr<std::set<OmSegment*> > selectedSegments_;

public:
	OmSegmentUncertainActionImpl(const OmID segmentationId,
								 boost::shared_ptr<std::set<OmSegment*> > selectedSegments,
								 const bool uncertain)
		: mSegmentationId( segmentationId )
		, uncertain_(uncertain)
		, selectedSegments_(selectedSegments)
	{}

	void Execute()
	{
		OmSegmentUncertain::SetAsUncertain(SegmentationDataWrapper(mSegmentationId),
										   selectedSegments_,
										   uncertain_);
		OmCacheManager::TouchFresheness();
	}

	void Undo()
	{
		OmSegmentUncertain::SetAsUncertain(SegmentationDataWrapper(mSegmentationId),
										   selectedSegments_,
										   !uncertain_);
		OmCacheManager::TouchFresheness();
	}

	std::string Description() const
	{
		QString lineItem;
		if(uncertain_) {
			lineItem = QString("Uncertain: ");
		} else {
			lineItem = QString("Not uncertain: ");
		}

		int count = 0;
		FOR_EACH(iter, *selectedSegments_){
			lineItem += QString("seg %1 + ").arg((*iter)->value());
			if(count > 10) break;
			count++;
		}

		return lineItem.toStdString();
	}

	QString classNameForLogFile() const {
		return "OmSegmentUncertainAction";
	}

private:
	template <typename T> friend class OmActionLoggerFSThread;
	friend QDataStream &operator<<(QDataStream&,
								   const OmSegmentUncertainActionImpl&);
	friend QDataStream &operator>>(QDataStream&,
								   OmSegmentUncertainActionImpl&);
};

#endif
