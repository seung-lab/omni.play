#include "gui/mainWindow/mainWindow.h"
#include "gui/toolbars/mainToolbar/refreshVolumeButton.h"
#include "utility/segmentationDataWrapper.hpp"

VolumeRefreshButton::VolumeRefreshButton(MainWindow * parent)
    : OmButton<MainWindow>( parent,
                            "Rebuild",
                            "Rebuild image and mesh data",
                            false)
{
    setFlat(true);
    setIconAndText(":/toolbars/mainToolbar/icons/1278008555_agt_softwareD.png");
}

void VolumeRefreshButton::doAction()
{
    const om::common::IDSet& set = SegmentationDataWrapper::ValidIDs();

    FOR_EACH(iter, set){
        printf("fixme!!!!!!!!!!!!!!\n");
    }
}
