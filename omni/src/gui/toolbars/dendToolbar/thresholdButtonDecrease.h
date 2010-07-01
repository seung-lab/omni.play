#ifndef THRESHOLD_BUTTON_DECREASE_H
#define THRESHOLD_BUTTON_DECREASE_H

#include "gui/widgets/thresholdGroup/omThresholdButtonDecrease.h"
#include "gui/toolbars/dendToolbar/thresholdGroup.h"

class ThresholdButtonDecrease : public OmThresholdButtonDecrease<ThresholdGroup>  {
 public:
	ThresholdButtonDecrease(ThresholdGroup * tg,
				const QString & title, 
				const QString & statusTip )
		: OmThresholdButtonDecrease<ThresholdGroup>( tg, title, statusTip){}
};

#endif
