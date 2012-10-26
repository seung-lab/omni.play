#pragma once

#include "common/common.h"
#include "project/omProject.h"
#include "system/cache/omCacheManager.h"
#include "segment/omSegmentUncertain.hpp"

class OmSegmentUncertainActionImpl{
private:
	SegmentationDataWrapper sdw_;
	bool uncertain_;
	om::shared_ptr<std::set<OmSegment*> > selectedSegments_;

public:
	OmSegmentUncertainActionImpl() {}
	OmSegmentUncertainActionImpl(const SegmentationDataWrapper& sdw,
								 om::shared_ptr<std::set<OmSegment*> > selectedSegments,
								 const bool uncertain)
		: sdw_(sdw)
		, uncertain_(uncertain)
		, selectedSegments_(selectedSegments)
	{}

	void Execute()
	{
		OmSegmentUncertain::SetAsUncertain(sdw_,
										   selectedSegments_,
										   uncertain_);
		OmCacheManager::TouchFreshness();
	}

	void Undo()
	{
		OmSegmentUncertain::SetAsUncertain(sdw_,
										   selectedSegments_,
										   !uncertain_);
		OmCacheManager::TouchFreshness();
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
	template <typename T> friend class OmActionLoggerThread;
	friend QDataStream &operator<<(QDataStream&,
								   const OmSegmentUncertainActionImpl&);
	friend QDataStream &operator>>(QDataStream&,
								   OmSegmentUncertainActionImpl&);
};

