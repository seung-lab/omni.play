#include "common/omDebug.h"
#include "gui/mainWindow/mainWindow.h"
#include "gui/toolbars/dendToolbar/graphTools.h"
#include "gui/toolbars/dendToolbar/splitButton.h"
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
    if(!isChecked()) {
        mParent->getViewGroupState()->Splitting()->EnterSplitMode();

    } else {
        mParent->getViewGroupState()->Splitting()->ExitSplitMode();
    }
}
