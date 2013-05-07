#include "viewGroup/omViewGroupState.h"
#include "common/omDebug.h"
#include "events/omEvents.h"
#include "gui/mainWindow/mainWindow.h"
#include "gui/sidebars/right/rightImpl.h"
#include "gui/sidebars/right/displayTools/displayTools.h"
#include "gui/sidebars/right/graphTools/graphTools.h"
#include "gui/sidebars/right/validationGroup/validationGroup.h"
#include "gui/sidebars/right/annotationGroup/annotationGroup.h"
#include "gui/widgets/omWidget.hpp"
#include "system/omAppState.hpp"
#include "utility/dataWrappers.h"
#include <QToolBox>

om::sidebars::rightImpl::rightImpl(MainWindow* mainWindow, OmViewGroupState* vgs)
    : QWidget(mainWindow)
    , mainWindow_(mainWindow)
    , vgs_(vgs)
{
    OmAppState::SetRightToolBar(this);

    graphTools_ = new GraphTools(this, vgs_);
    ValidationGroup* validationGroup = new ValidationGroup(this, vgs_);
    DisplayTools* displayTools = new DisplayTools(this, vgs_);
    AnnotationGroup* annotationGroup = new AnnotationGroup(this, vgs_);

    QToolBox* tbox = new QToolBox(this);
    tbox->addItem(graphTools_, graphTools_->getName());
    tbox->addItem(validationGroup, validationGroup->getName());
    tbox->addItem(displayTools, displayTools->getName());
    tbox->addItem(annotationGroup, annotationGroup->getName());

    QVBoxLayout* vbox = new QVBoxLayout(this);
    vbox->addWidget(tbox);
    setMaximumSize(250, 2000);
}

SegmentationDataWrapper om::sidebars::rightImpl::GetSDW(){
    return vgs_->Segmentation();
}

void om::sidebars::rightImpl::updateGui(){
    OmEvents::Redraw2d();
}

void om::sidebars::rightImpl::SetSplittingOff(){
    graphTools_->SetSplittingOff();
}

void om::sidebars::rightImpl::SetShatteringOff(){
    graphTools_->SetShatteringOff();
}
