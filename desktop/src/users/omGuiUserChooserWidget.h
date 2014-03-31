#pragma once
#include "precomp.h"

#include "datalayer/paths.hpp"

class OmGuiUserChooser;

namespace om {
namespace gui {

class userChooserWidget : public QWidget {
 private:
  OmGuiUserChooser* const chooser_;

  std::vector<std::string> users_;

 public:
  userChooserWidget(OmGuiUserChooser* chooser, om::file::path userFolder);

  void Done();

 private:
  void setup();
  static std::vector<std::string> findUsers(om::file::path userFolder);
};

}  // namespace gui
}  // namespace om
