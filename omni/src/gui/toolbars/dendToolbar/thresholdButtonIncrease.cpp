#include "gui/toolbars/dendToolbar/thresholdButtonIncrease.h"
#include "common/omDebug.h"
#include "gui/toolbars/dendToolbar/dendToolbar.h"

ThresholdButtonIncrease::ThresholdButtonIncrease(ThresholdGroup * tg,
						 const QString & title, 
						 const QString & statusTip )
	: OmButton<ThresholdGroup>( tg, title, statusTip, false)
{
}

void ThresholdButtonIncrease::doAction()
{
        debug("dendbar", "DendToolBar::increaseThreshold\n");
	mParent->increaseThresholdByEpsilon();
	mParent->updateGui();
}
