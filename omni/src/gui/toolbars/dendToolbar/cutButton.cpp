#include "common/omDebug.h"
#include "gui/mainWindow/mainWindow.h"
#include "gui/toolbars/dendToolbar/graphTools.h"
#include "gui/toolbars/dendToolbar/cutButton.h"
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
        mParent->getViewGroupState()->Cutting()->EnterCutMode();

    } else {
        mParent->getViewGroupState()->Cutting()->ExitCutMode();
    }
}
