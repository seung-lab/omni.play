#ifndef BREAK_THRESHOLD_GROUP_H
#define BREAK_THRESHOLD_GROUP_H

#include "gui/widgets/omSpinBox.hpp"
#include "common/omDebug.h"
#include "gui/toolbars/dendToolbar/graphTools.h"
#include "viewGroup/omViewGroupState.h"

class BreakThresholdGroup : public OmSpinBox {
 Q_OBJECT
 public:
	BreakThresholdGroup(GraphTools * d)
	: OmSpinBox(d, om::DONT_UPDATE_AS_TYPE)
	, mParent(d)
{
	setSingleStep(0.02);
	setMaximum(1.0);
	setInitialGUIThresholdValue();
}

 private:
	GraphTools *const mParent;

	void setInitialGUIThresholdValue()
	{
		const double defaultThreshold = 0.95;

		setGUIvalue(defaultThreshold);

		if( NULL != mParent->getViewGroupState() ) {
			mParent->getViewGroupState()->setBreakThreshold(defaultThreshold);
		}
	}

	void actUponThresholdChange( const float threshold )
	{
		if( NULL != mParent->getViewGroupState() ) {
			mParent->getViewGroupState()->setBreakThreshold( threshold );
		}
	}
};

#endif
