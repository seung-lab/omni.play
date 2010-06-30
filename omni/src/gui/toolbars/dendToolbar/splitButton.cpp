#include "common/omDebug.h"
#include "gui/mainwindow.h"
#include "gui/toolbars/dendToolbar/splitButton.h"
#include "system/omStateManager.h"
#include "system/viewGroup/omViewGroupState.h"

SplitButton::SplitButton(DendToolBar * mw, 
			 const QString & title, 
			 const QString & statusTip,
			 const bool isCheckable )
	: OmButton<DendToolBar>( mw, title, statusTip, isCheckable)
{
}

void SplitButton::doAction()
{
        debug("dendbar", "DendToolBar::split(%i)\n", isChecked());
	if(!isChecked()) {
		mParent->getViewGroupState()->SetShowSplitMode(true);
		OmStateManager::SetToolModeAndSendEvent(SPLIT_MODE);
	} else {
        	debug("dendbar", "unchecking\n");
		mParent->getViewGroupState()->SetSplitMode(false, false);
		OmStateManager::SetOldToolModeAndSendEvent();
	}

}
