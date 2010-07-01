#ifndef THRESHOLD_BUTTON_INCREASE_H
#define THRESHOLD_BUTTON_INCREASE_H

#include "gui/widgets/thresholdGroup/omThresholdButtonIncrease.h"
#include "gui/toolbars/dendToolbar/thresholdGroup.h"

class ThresholdButtonIncrease : public OmThresholdButtonIncrease<ThresholdGroup>  {
 public:
	ThresholdButtonIncrease(ThresholdGroup * tg,
				const QString & title, 
				const QString & statusTip )
		: OmThresholdButtonIncrease<ThresholdGroup>( tg, title, statusTip){}
};

#endif
