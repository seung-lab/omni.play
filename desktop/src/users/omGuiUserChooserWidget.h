#pragma once

#include <QWidget>

class OmGuiUserChooser;

namespace om {
namespace gui {

class userChooserWidget : public QWidget {
 private:
  OmGuiUserChooser* const chooser_;

  std::vector<std::string> users_;

 public:
  userChooserWidget(OmGuiUserChooser* chooser);

  void Done();

 private:
  void setup();
  static std::vector<std::string> findUsers();
};

}  // namespace gui
}  // namespace om
