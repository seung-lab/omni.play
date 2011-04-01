#ifndef OPEN_VIEW_GROUP_BUTTON_H
#define OPEN_VIEW_GROUP_BUTTON_H

#include "gui/mainWindow/mainWindow.h"
#include "gui/widgets/omButton.hpp"
#include "viewGroup/omViewGroupState.h"

class OpenViewGroupButton : public OmButton<MainWindow> {
public:
    OpenViewGroupButton(MainWindow * mw)
        : OmButton<MainWindow>( mw,
                                "Display Views",
                                "Open the 2D and 3D views",
                                false)
    {
        setFlat(true);
        setIconAndText(":/toolbars/mainToolbar/icons/1278009384_kllckety.png");
    }

private:
    void doAction()
    {
        const OmID channelID = 1;
        const OmID segmentationID = 1;

        mParent->getViewGroupState()->AddAllViews( channelID, segmentationID );
    }
};

#endif
