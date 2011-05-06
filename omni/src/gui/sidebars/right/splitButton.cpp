#include "common/omDebug.h"
#include "gui/mainWindow/mainWindow.h"
#include "gui/sidebars/right/graphTools.h"
#include "gui/sidebars/right/splitButton.h"
#include "viewGroup/omViewGroupState.h"
#include "viewGroup/omSplitting.hpp"
#include "system/omConnect.hpp"

SplitButton::SplitButton(GraphTools * d)
    : OmButton<GraphTools>( d,
                            "Split",
                            "Split object mode",
                            true)
{
    om::connect(this, SIGNAL(clicked(bool)),
                this, SLOT(enterOrExitSplitMode(bool)));
}

void SplitButton::enterOrExitSplitMode(const bool inSplitMode)
{
    if(inSplitMode){
        mParent->GetViewGroupState()->Splitting()->EnterSplitMode();

    } else {
        mParent->GetViewGroupState()->Splitting()->ExitSplitMode();
    }
}
