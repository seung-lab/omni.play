#include "common/omDebug.h"
#include "gui/mainWindow/mainWindow.h"
#include "gui/sidebars/right/graphTools.h"
#include "gui/sidebars/right/cutButton.h"
#include "viewGroup/omCutting.hpp"
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
    if(!isChecked()){
        mParent->GetViewGroupState()->Cutting()->EnterCutMode();

    } else {
        mParent->GetViewGroupState()->Cutting()->ExitCutMode();
    }
}
