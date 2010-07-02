#ifndef DUST_3D_THRESHOLD_GROUP_H
#define DUST_3D_THRESHOLD_GROUP_H

#include "gui/widgets/thresholdGroup/omThresholdGroup.h"

class Dust3DThresholdGroup : public OmThresholdGroup {
 Q_OBJECT 
 public:
	Dust3DThresholdGroup(DendToolBar * parent);
	float getThresholdEpsilon();

 private:
	void addToThreshold(const float num);
	void setThresholdValue();
	void actUponThresholdChange( const float threshold );
	float filterUserEnteredThreshold(const float threshold);
};

#endif
