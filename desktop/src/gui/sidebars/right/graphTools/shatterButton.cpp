#include "common/omDebug.h"
#include "gui/sidebars/right/graphTools/shatterButton.h"
#include "gui/sidebars/right/graphTools/graphTools.h"
#include "system/cache/omCacheManager.h"
#include "system/omConnect.hpp"
#include "viewGroup/omViewGroupState.h"

ShatterButton::ShatterButton(GraphTools* d)
    : OmButton<GraphTools>( d,
                            "Shatter",
                            "Shatter object mode",
                            true)
{
    om::connect(this, SIGNAL(clicked(bool)),
                this, SLOT(enterOrExitShatterMode(bool)));
}

void ShatterButton::enterOrExitShatterMode(const bool inShatterMode)
{
    // if(inShatterMode){
    //     mParent->GetViewGroupState()->Shatterting()->EnterShatterMode();

    // } else {
    //     mParent->GetViewGroupState()->Shatterting()->ExitShatterMode();
    // }
}
