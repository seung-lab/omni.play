#include "common/omDebug.h"
#include "gui/toolbars/dendToolbar/breakButton.h"
#include "system/omCacheManager.h"
#include "system/viewGroup/omViewGroupState.h"

BreakButton::BreakButton(DendToolBar * d)
	: OmButton<DendToolBar>( d,
				 "Break", 
				 "Shatter object mode", 
				 true)
{
}

void BreakButton::doAction()
{
	OmCacheManager::Freshen(true);
	mParent->getViewGroupState()->ToggleShatterMode();

	mParent->updateGui();
}
