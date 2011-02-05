#include "common/omDebug.h"
#include "gui/mainwindow.h"
#include "gui/toolbars/dendToolbar/graphTools.h"
#include "gui/toolbars/dendToolbar/splitButton.h"
#include "system/omStateManager.h"
#include "viewGroup/omViewGroupState.h"
#include "viewGroup/omSplitting.hpp"

SplitButton::SplitButton(GraphTools * d)
    : OmButton<GraphTools>( d,
                            "Split",
                            "Split object mode",
                            true)
{
}

void SplitButton::doAction()
{
    //debug(dendbar, "DendToolBar::split(%i)\n", isChecked());
    if(!isChecked()) {
        mParent->getViewGroupState()->Splitting()->SetShowSplitMode(true);
        OmStateManager::SetToolModeAndSendEvent(SPLIT_MODE);
    } else {
        //debug(dendbar, "unchecking\n");
        mParent->getViewGroupState()->Splitting()->SetSplitMode(false, false);
    }
}
