#include "common/omDebug.h"
#include "gui/toolbars/dendToolbar/breakButton.h"
#include "system/cache/omCacheManager.h"
#include "system/viewGroup/omViewGroupState.h"
#include "gui/toolbars/dendToolbar/graphTools.h"

BreakButton::BreakButton(GraphTools * d)
	: OmButton<GraphTools>( d,
				 "Break",
				 "Shatter object mode",
				 true)
{
}

void BreakButton::doAction()
{
	OmCacheManager::TouchFresheness();
	mParent->getViewGroupState()->ToggleShatterMode();

	mParent->updateGui();
}
