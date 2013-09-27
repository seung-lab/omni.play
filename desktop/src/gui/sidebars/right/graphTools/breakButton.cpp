#include "common/logging.h"
#include "gui/sidebars/right/graphTools/breakButton.h"
#include "gui/sidebars/right/graphTools/graphTools.h"
#include "system/cache/omCacheManager.h"
#include "system/omConnect.hpp"
#include "viewGroup/omViewGroupState.h"

BreakButton::BreakButton(GraphTools* d)
    : OmButton<GraphTools>( d,
                            "Break",
                            "Shatter object mode",
                            true)
{
    om::connect(this, SIGNAL(clicked(bool)),
                this, SLOT(showOrHideBreakThresholdBox(bool)));
}

void BreakButton::doAction()
{
    mParent->GetViewGroupState()->ToggleShatterMode();
    mParent->updateGui();
}

void BreakButton::showOrHideBreakThresholdBox(const bool show)
{
    if(show){
        mParent->ShowBreakThreasholdBox();

    } else {
        mParent->HideBreakThreasholdBox();
    }
}
