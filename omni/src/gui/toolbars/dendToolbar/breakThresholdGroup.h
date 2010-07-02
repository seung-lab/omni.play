#ifndef BREAK_THRESHOLD_GROUP_H
#define BREAK_THRESHOLD_GROUP_H

#include "gui/widgets/thresholdGroup/omThresholdGroup.h"

class BreakThresholdGroup : public OmThresholdGroup {
 Q_OBJECT 
 public:
	BreakThresholdGroup(DendToolBar * parent);
	float getThresholdEpsilon();

 private:
	void addToThreshold(const float num);
	void setThresholdValue();
	void actUponThresholdChange( const float threshold );
	float filterUserEnteredThreshold(const float threshold);
};

#endif
