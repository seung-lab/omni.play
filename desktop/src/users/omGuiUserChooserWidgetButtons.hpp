#pragma once

#include "gui/widgets/omButton.hpp"
#include "users/omGuiUserChooserWidget.h"
#include "users/omUsers.h"

namespace om {
namespace gui {

class chooseUserButton : public OmButton<om::gui::userChooserWidget> {
 private:
  userChooserWidget* const chooser_;
  const std::string userName_;

 public:
  chooseUserButton(userChooserWidget* chooser, const std::string& userName)
      : OmButton<om::gui::userChooserWidget>(chooser,
                                             QString::fromStdString(userName),
                                             QString::fromStdString(userName),
                                             false),
        chooser_(chooser),
        userName_(userName) {}

 private:
  void doAction() {
    OmProject::Globals().Users().SwitchToUser(userName_);
    chooser_->Done();
  }
};

}  // namespace gui
}  // namespace om
