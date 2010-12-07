#ifndef BREAK_THRESHOLD_GROUP_H
#define BREAK_THRESHOLD_GROUP_H

#include "gui/widgets/omThresholdGroup.h"
class GraphTools;

class BreakThresholdGroup : public OmThresholdGroup {
 Q_OBJECT 
 public:
	BreakThresholdGroup(GraphTools * parent);

 private:
	GraphTools *const mParent;
	void setInitialGUIThresholdValue();
	void actUponThresholdChange( const float threshold );
};

#endif
