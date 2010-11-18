#ifndef THRESHOLD_GROUP_H
#define THRESHOLD_GROUP_H

#include "actions/omActions.hpp"
#include "common/omDebug.h"
#include "gui/toolbars/dendToolbar/graphTools.h"
#include "gui/toolbars/dendToolbar/thresholdGroup.h"
#include "gui/widgets/omDoubleSpinBox.hpp"
#include "system/omEvents.h"
#include "utility/dataWrappers.h"
#include "volume/omSegmentation.h"

class ThresholdGroup : public OmDoubleSpinBox {
Q_OBJECT
public:
	ThresholdGroup(GraphTools * d)
		: OmDoubleSpinBox(d, om::DONT_UPDATE_AS_TYPE)
		, mParent(d)
	{
		setSingleStep(0.002);
		setMaximum(1.0);
		setDecimals(3);
		setInitialGUIThresholdValue();
	}

private:
	GraphTools *const mParent;

	void setInitialGUIThresholdValue()
	{
		double threshold = 0.95;

		SegmentationDataWrapper sdw = mParent->GetSegmentationDataWrapper();
		if(sdw.IsSegmentationValid()){
			threshold = sdw.GetSegmentation().GetDendThreshold();
		}

		setGUIvalue(threshold);
	}

	void actUponThresholdChange( const float threshold )
	{
		SegmentationDataWrapper sdw = mParent->GetSegmentationDataWrapper();
		if(!sdw.IsSegmentationValid()){
			return;
		}

		OmActions::ChangeMSTthreshold(sdw.GetSegmentationID(), threshold);
	}
};

#endif
