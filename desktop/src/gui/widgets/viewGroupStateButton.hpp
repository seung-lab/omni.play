#pragma once
#include "precomp.h"

#include "gui/widgets/omButton.hpp"
#include "viewGroup/omViewGroupState.h"
#include "utility/segmentationDataWrapper.hpp"

class ViewGroupStateButton : public OmButton {
 public:
  ViewGroupStateButton(QWidget* mw, const QString& title,
      const QString& statusTip, const bool isCheckable, OmViewGroupState& vgs)
      : OmButton(mw, title, statusTip, isCheckable), vgs_(vgs) {};

  OmViewGroupState& GetViewGroupState() const {
    return vgs_;
  }

 private:
  OmViewGroupState& vgs_;
};
