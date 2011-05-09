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

QDockWidget* ViewGroup::AddView2Dchannel(const ChannelDataWrapper& chanID, const ViewType viewType){
    return impl_->AddView2Dchannel(chanID, viewType);
}

std::pair<QDockWidget*, QDockWidget*>
ViewGroup::AddView2Dsegmentation(const SegmentationDataWrapper& sdw, const ViewType viewType){
    return impl_->AddView2Dsegmentation(sdw, viewType);
}

void ViewGroup::AddXYView(){
    return impl_->AddXYView();
}

void ViewGroup::AddAllViews(){
    return impl_->AddAllViews();
}

void ViewGroup::AddXYViewAndView3d(){
    return impl_->AddXYViewAndView3d();
}
