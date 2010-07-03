#ifndef THRESHOLD_GROUP_H
#define THRESHOLD_GROUP_H

#include "gui/widgets/thresholdGroup/omThresholdGroup.h"

class GraphTools;

class ThresholdGroup : public OmThresholdGroup {
 Q_OBJECT 
 public:
	ThresholdGroup(GraphTools * parent);

 private:
	GraphTools *const mParent;
	void setInitialGUIThresholdValue();
	void actUponThresholdChange( const float threshold );
};

#endif
