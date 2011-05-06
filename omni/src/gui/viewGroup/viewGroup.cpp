#include "gui/viewGroup/viewGroup.h"
#include "gui/viewGroup/viewGroupImpl.hpp"

ViewGroup::ViewGroup(MainWindow* mainWindow, OmViewGroupState* vgs)
    : mainWindow_(mainWindow)
    , impl_(new om::gui::viewGroupImpl(mainWindow, vgs))
{}

ViewGroup::~ViewGroup()
{}

void ViewGroup::AddView3D(){
    impl_->AddView3D();
}

void ViewGroup::AddView3D4View(){
    impl_->AddView3D4View();
}

QDockWidget* ViewGroup::AddView2Dchannel(const OmID chanID, const ViewType viewType){
    return impl_->AddView2Dchannel(chanID, viewType);
}

std::pair<QDockWidget*, QDockWidget*>
ViewGroup::AddView2Dsegmentation(const OmID segmentationID, const ViewType viewType){
    return impl_->AddView2Dsegmentation(segmentationID, viewType);
}

void ViewGroup::AddXYView(const OmID channelID, const OmID segmentationID){
    return impl_->AddXYView(channelID, segmentationID);
}

void ViewGroup::AddAllViews(const OmID channelID, const OmID segmentationID){
    return impl_->AddAllViews(channelID, segmentationID);
}

void ViewGroup::AddXYViewAndView3d(const OmID channelID, const OmID segmentationID){
    return impl_->AddXYViewAndView3d(channelID, segmentationID);
}
