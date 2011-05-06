#ifndef OPEN_SINGLE_VIEW_BUTTON_HPP
#define OPEN_SINGLE_VIEW_BUTTON_HPP

#include "gui/mainWindow/mainWindow.h"
#include "gui/widgets/omButton.hpp"
#include "viewGroup/omViewGroupState.h"
#include "gui/viewGroup/viewGroup.h"

class OpenSingleViewButton : public OmButton<MainWindow> {
public:
    OpenSingleViewButton(MainWindow * mw)
        : OmButton<MainWindow>( mw,
                                "Display View",
                                "Open the 2D XY Views",
                                false)
    {
        setFlat(true);
        setIconAndText(":/toolbars/mainToolbar/icons/1299635956_window_new.png");
    }

private:
    void doAction()
    {
        const OmID channelID = 1;
        const OmID segmentationID = 1;

        mParent->GetViewGroupState()->GetViewGroup()->AddXYView( channelID, segmentationID );
    }
};

#endif
