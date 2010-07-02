#ifndef DUST_3D_THRESHOLD_GROUP_H
#define DUST_3D_THRESHOLD_GROUP_H

#include "gui/widgets/thresholdGroup/omThresholdGroup.h"

class Dust3DThresholdGroup : public OmThresholdGroup {
 Q_OBJECT 
 public:
	Dust3DThresholdGroup(DendToolBar * parent);

 private:
	void setInitialGUIThresholdValue();
	void actUponThresholdChange( const float threshold );
};

#endif
