#include "common/omDebug.h"
#include "gui/mainwindow.h"
#include "gui/toolbars/dendToolbar/graphTools.h"
#include "gui/toolbars/dendToolbar/cutButton.h"
#include "system/omStateManager.h"
#include "viewGroup/omViewGroupState.h"

CutButton::CutButton(GraphTools * d)
	: OmButton<GraphTools>( d, 
				 "Cut", 
				 "Cut object mode", 
				 true)
{
}

void CutButton::doAction()
{
        //debug(dendbar, "DendToolBar::split(%i)\n", isChecked());
	if(!isChecked()) {
		mParent->getViewGroupState()->SetShowCutMode(true);
		OmStateManager::SetToolModeAndSendEvent(CUT_MODE);
	} else {
        	//debug(dendbar, "unchecking\n");
		mParent->getViewGroupState()->SetCutMode(false, false);
	}
}
