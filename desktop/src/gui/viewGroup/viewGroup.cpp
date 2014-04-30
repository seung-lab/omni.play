#include "gui/viewGroup/viewGroup.h"
#include "gui/viewGroup/viewGroupImpl.hpp"

ViewGroup::ViewGroup(MainWindow* mainWindow, OmViewGroupState& vgs)
    : impl_(new om::gui::viewGroupImpl(mainWindow, vgs)) {}

ViewGroup::~ViewGroup() {}

void ViewGroup::AddView3D() { impl_->AddView3D(); }

void ViewGroup::AddView3D4View() { impl_->AddView3D4View(); }

void ViewGroup::AddView2Dchannel(const ChannelDataWrapper& chanID,
                                 const om::common::ViewType viewType) {
  impl_->AddView2Dchannel(chanID, viewType);
}

void ViewGroup::AddView2Dsegmentation(const SegmentationDataWrapper& sdw,
                                      const om::common::ViewType viewType) {
  impl_->AddView2Dsegmentation(sdw, viewType);
}

void ViewGroup::AddXYView() { impl_->AddXYView(); }

void ViewGroup::AddAllViews() { impl_->AddAllViews(); }

void ViewGroup::AddXYViewAndView3d() { impl_->AddXYViewAndView3d(); }
