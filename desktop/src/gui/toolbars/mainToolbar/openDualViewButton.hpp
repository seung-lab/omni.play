#pragma once

#include "gui/mainWindow/mainWindow.h"
#include "gui/widgets/omButton.hpp"
#include "viewGroup/omViewGroupState.h"
#include "gui/viewGroup/viewGroup.h"

class OpenDualViewButton : public OmButton<MainWindow> {
public:
    OpenDualViewButton(MainWindow * mw)
        : OmButton<MainWindow>( mw,
                                "Display Views",
                                "Open the 2D and 3D views",
                                false)
    {
        setFlat(true);
        setIconAndText(":/toolbars/mainToolbar/icons/1304368359_MyNetworkPlaces.png");
    }

private:
    void doAction(){
        mParent->GetViewGroupState()->GetViewGroup()->AddXYViewAndView3d();
    }
};
