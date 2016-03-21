#pragma once
#include "precomp.h"
#include "gui/widgets/omWidget.hpp"
#include "viewGroup/omViewGroupState.h"

class ViewGroupStateWidget : public OmWidget {
 public:
  ViewGroupStateWidget(QWidget* widget, OmViewGroupState& vgs) : OmWidget(widget), vgs_(vgs) {}

  OmViewGroupState& GetViewGroupState() {
    return vgs_;
  }

 private:
  OmViewGroupState& vgs_;
};
