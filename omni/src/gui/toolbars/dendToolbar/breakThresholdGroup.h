#ifndef BREAK_THRESHOLD_GROUP_H
#define BREAK_THRESHOLD_GROUP_H

#include "gui/widgets/thresholdGroup/omThresholdGroup.h"

class BreakThresholdGroup : public OmThresholdGroup {
 Q_OBJECT 
 public:
	BreakThresholdGroup(DendToolBar * parent);

 private:
	void setInitialGUIThresholdValue();
	void actUponThresholdChange( const float threshold );
};

#endif
