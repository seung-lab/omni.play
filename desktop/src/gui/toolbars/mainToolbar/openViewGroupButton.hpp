#pragma once
#include "precomp.h"

#include "gui/mainWindow/mainWindow.h"
#include "gui/widgets/viewGroupStateButton.hpp"
#include "viewGroup/omViewGroupState.h"
#include "gui/viewGroup/viewGroup.h"

namespace om {
  namespace gui {
    /*
     * This enumeration indicates the view group configuration.
     * The correct way to do this would to have a modular configuration via
     * the ViewGroup class but it will require a lot more refactoring than 
     * i want to commit to right now.
     */
    enum class ViewConfiguration {
      ALL, SINGLE, DUAL
    };
  }
}

class OpenViewGroupButton : public ViewGroupStateButton {
 public:
  OpenViewGroupButton(QWidget* mw, OmViewGroupState& vgs,
      const om::gui::ViewConfiguration viewConfiguration)
      : ViewGroupStateButton(mw, getTitle(viewConfiguration),
          getStatusTip(viewConfiguration), false, vgs),
        viewConfiguration_(viewConfiguration) {
    setFlat(true);
    setIconAndText(getIcon(viewConfiguration));
  }

 private:
  const om::gui::ViewConfiguration viewConfiguration_;

  const QString getTitle(om::gui::ViewConfiguration viewConfiguration) {
    switch(viewConfiguration_) {
      case om::gui::ViewConfiguration::SINGLE:
        return "Display Single View";
      case om::gui::ViewConfiguration::DUAL:
        return "Display Dual Views";
      case om::gui::ViewConfiguration::ALL:
      default:
        return "Display All Views";
    }
  }

  const QString getStatusTip(om::gui::ViewConfiguration viewConfiguration) {
    switch(viewConfiguration_) {
      case om::gui::ViewConfiguration::SINGLE:
        return "Open Single 2D XY View";
        break;
      case om::gui::ViewConfiguration::DUAL:
        return "Open 2D XY and 3D View";
      case om::gui::ViewConfiguration::ALL:
      default:
        return "Display All 2D and 3D Views";
    }
  }

  const QString getIcon(om::gui::ViewConfiguration viewConfiguration) {
    switch(viewConfiguration_) {
      case om::gui::ViewConfiguration::SINGLE:
        return ":/toolbars/mainToolbar/icons/1299635956_window_new.png";
      case om::gui::ViewConfiguration::DUAL:
        return ":/toolbars/mainToolbar/icons/1304368359_MyNetworkPlaces.png";
      case om::gui::ViewConfiguration::ALL:
      default:
        return ":/toolbars/mainToolbar/icons/1278009384_kllckety.png";
    }
  }

  void onLeftClick() override {
    switch(viewConfiguration_) {
      case om::gui::ViewConfiguration::SINGLE:
        GetViewGroupState().GetViewGroup().AddXYView();
        break;
      case om::gui::ViewConfiguration::DUAL:
        GetViewGroupState().GetViewGroup().AddXYViewAndView3d();
        break;
      case om::gui::ViewConfiguration::ALL:
      default:
        GetViewGroupState().GetViewGroup().AddAllViews();
        break;
    }
  }
};
