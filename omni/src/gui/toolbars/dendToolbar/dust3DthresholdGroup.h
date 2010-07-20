#ifndef DUST_3D_THRESHOLD_GROUP_H
#define DUST_3D_THRESHOLD_GROUP_H

#include "gui/widgets/omThresholdGroup.h"
class DisplayTools;

class Dust3DThresholdGroup : public OmThresholdGroup {
 Q_OBJECT 
 public:
	Dust3DThresholdGroup(DisplayTools * parent);

 private:
	DisplayTools *const mParent;

	void setInitialGUIThresholdValue();
	void actUponThresholdChange( const float threshold );
};

#endif
