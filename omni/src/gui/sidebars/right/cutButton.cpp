#include "common/omDebug.h"
#include "gui/mainWindow/mainWindow.h"
#include "gui/sidebars/right/cutButton.h"
#include "gui/sidebars/right/graphTools.h"
#include "system/omConnect.hpp"
#include "viewGroup/omCutting.hpp"
#include "viewGroup/omViewGroupState.h"

CutButton::CutButton(GraphTools * d)
    : OmButton<GraphTools>( d,
                            "Cut",
                            "Cut object mode",
                            true)
{
    om::connect(this, SIGNAL(clicked(bool)),
                this, SLOT(enableCutMode(bool)));
}

void CutButton::enableCutMode(const bool enableCutMode)
{
    if(enableCutMode){
        mParent->GetViewGroupState()->Cutting()->EnterCutMode();

    } else {
        mParent->GetViewGroupState()->Cutting()->ExitCutMode();
    }
}
