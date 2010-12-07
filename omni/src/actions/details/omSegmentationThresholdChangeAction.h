#ifndef OM_SEGMENTATION_THRESHOLD_CHANGE_ACTION_H
#define OM_SEGMENTATION_THRESHOLD_CHANGE_ACTION_H

#include "actions/details/omAction.h"
#include "common/omCommon.h"

class OmSegmentationThresholdChangeActionImpl;

class OmSegmentationThresholdChangeAction : public OmAction {
public:
	OmSegmentationThresholdChangeAction(boost::shared_ptr<OmSegmentationThresholdChangeActionImpl> impl)
		: impl_(impl)
	{}

	OmSegmentationThresholdChangeAction(const OmID segmentationId,
										const float threshold);

private:
	void Action();
	void UndoAction();
	std::string Description();
	void save(const std::string &);

	boost::shared_ptr<OmSegmentationThresholdChangeActionImpl> impl_;

};

#endif
