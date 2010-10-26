#ifndef OM_SEGMENTATION_THRESHOLD_CHANGE_ACTION_H
#define OM_SEGMENTATION_THRESHOLD_CHANGE_ACTION_H

#include "system/omAction.h"

class OmSegmentationThresholdChangeActionImpl;

class OmSegmentationThresholdChangeAction : public OmAction {
public:
	OmSegmentationThresholdChangeAction( const OmID segmentationId, const float threshold);

private:
	void Action();
	void UndoAction();
	std::string Description();
	void save(const std::string &);

	boost::shared_ptr<OmSegmentationThresholdChangeActionImpl> impl_;

};

#endif
