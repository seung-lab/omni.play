#ifndef THRESHOLD_GROUP_H
#define THRESHOLD_GROUP_H

#include "gui/widgets/thresholdGroup/omThresholdGroup.h"

class ThresholdGroup : public OmThresholdGroup {
 Q_OBJECT 
 public:
	ThresholdGroup(DendToolBar * parent);

 private:
	void setInitialGUIThresholdValue();
	void actUponThresholdChange( const float threshold );
};

#endif
