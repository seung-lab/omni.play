#include "viewGroup/omViewGroupState.h"
#include "common/omDebug.h"
#include "events/omEvents.h"
#include "gui/mainWindow/mainWindow.h"
#include "gui/sidebars/right/dendToolbar.h"
#include "gui/sidebars/right/displayTools/displayTools.h"
#include "gui/sidebars/right/graphTools.h"
#include "gui/sidebars/right/validationGroup.h"
#include "gui/widgets/omWidget.hpp"
#include "system/omAppState.hpp"
#include "utility/dataWrappers.h"

DendToolBar::DendToolBar(MainWindow* mainWindow, OmViewGroupState* vgs)
    : QWidget(mainWindow)
    , mainWindow_(mainWindow)
    , vgs_(vgs)
{
    OmAppState::SetDendToolBar(this);

    graphTools_ = new GraphTools(this, vgs_);
    ValidationGroup* validationGroup = new ValidationGroup(this, vgs_);
    DisplayTools* displayTools = new DisplayTools(this, vgs_);

    QVBoxLayout* vbox = new QVBoxLayout(this);
    vbox->addWidget(wrapWithGroupBox(graphTools_));
    vbox->addWidget(wrapWithGroupBox(validationGroup));
    vbox->addWidget(wrapWithGroupBox(displayTools));
    vbox->addStretch(1);
}

SegmentationDataWrapper DendToolBar::GetSDW(){
    return vgs_->Segmentation();
}

void DendToolBar::updateGui(){
    OmEvents::Redraw2d();
}

void DendToolBar::SetSplittingOff(){
    graphTools_->SetSplittingOff();
}

void DendToolBar::SetCuttingOff(){
    graphTools_->SetCuttingOff();
}

void DendToolBar::RefreshThreshold(){
    graphTools_->RefreshThreshold();
}
