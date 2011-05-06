#include "common/omDebug.h"
#include "gui/mainWindow/mainWindow.h"
#include "gui/sidebars/right/graphTools.h"
#include "gui/sidebars/right/splitButton.h"
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
        mParent->GetViewGroupState()->Splitting()->EnterSplitMode();

    } else {
        mParent->GetViewGroupState()->Splitting()->ExitSplitMode();
    }
}
