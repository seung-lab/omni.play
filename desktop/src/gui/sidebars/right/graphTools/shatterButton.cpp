#include "common/omDebug.h"
#include "gui/sidebars/right/graphTools/shatterButton.h"
#include "gui/sidebars/right/graphTools/graphTools.h"
#include "system/omStateManager.h"
#include "system/omConnect.hpp"
#include "events/omEvents.h"
#include "viewGroup/omViewGroupState.h"
#include "gui/tools.hpp"

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
	if(inShatterMode) {
		OmStateManager::SetToolModeAndSendEvent(om::tool::SHATTER);
	} else {
		OmStateManager::SetOldToolModeAndSendEvent();
	}

    OmEvents::Redraw3d();
    OmEvents::Redraw2d();
}
