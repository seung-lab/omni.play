#include "gui/toolbars/dendToolbar/thresholdButtonDecrease.h"
#include "common/omDebug.h"
#include "gui/toolbars/dendToolbar/dendToolbar.h"

ThresholdButtonDecrease::ThresholdButtonDecrease(ThresholdGroup * tg,
						 const QString & title, 
						 const QString & statusTip )
	: OmButton<ThresholdGroup>( tg, title, statusTip, false)
{
}

void ThresholdButtonDecrease::doAction()
{
        debug("dendbar", "DendToolBar::increaseThreshold\n");
	mParent->decreaseThresholdByEpsilon();
	mParent->updateGui();
}
