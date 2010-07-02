#ifndef THRESHOLD_GROUP_H
#define THRESHOLD_GROUP_H

#include "gui/widgets/thresholdGroup/omThresholdGroup.h"

class ThresholdGroup : public OmThresholdGroup {
 Q_OBJECT 
 public:
	ThresholdGroup(DendToolBar * parent);
	float getThresholdEpsilon();

 private:
	void addToThreshold(const float num);
	void setThresholdValue();
	void actUponThresholdChange( const float threshold );
	float filterUserEnteredThreshold(const float threshold);
};

#endif
