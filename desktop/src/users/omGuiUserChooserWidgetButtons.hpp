#pragma once
#include "precomp.h"

#include "gui/widgets/omButton.hpp"
#include "users/omGuiUserChooserWidget.h"
#include "users/omUsers.h"

namespace om {
namespace gui {

class chooseUserButton : public OmButton {
 private:
  userChooserWidget* const chooser_;
  const std::string userName_;

 public:
  chooseUserButton(userChooserWidget* chooser, const std::string& userName)
      : OmButton(chooser,
                                             QString::fromStdString(userName),
                                             QString::fromStdString(userName),
                                             false),
        chooser_(chooser),
        userName_(userName) {}

 private:
  void onLeftClick() override {
    OmProject::Globals().Users().SwitchToUser(userName_);
    chooser_->Done();
  }
};

}  // namespace gui
}  // namespace om
