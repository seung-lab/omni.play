#include "project/omProject.h"
#include "project/omProjectGlobals.h"
#include "users/omGuiUserChooser.h"
#include "users/omGuiUserChooserWidget.h"
#include "users/omGuiUserChooserWidgetButtons.hpp"
#include "users/omUsers.h"

om::gui::userChooserWidget::userChooserWidget(OmGuiUserChooser* chooser)
    : QWidget(chooser), chooser_(chooser) {
  users_ = findUsers();

  if (!users_.size()) {
    return;
  }

  setup();
}

void om::gui::userChooserWidget::setup() {
  QVBoxLayout* box = new QVBoxLayout;

  box->addWidget(new QLabel("Choose User", this));

  FOR_EACH(iter, users_) { box->addWidget(new chooseUserButton(this, *iter)); }

  setLayout(box);
}

std::vector<std::string> om::gui::userChooserWidget::findUsers() {
  om::file::path path = OmProject::Globals().Users().UserPaths();

  std::vector<std::string> ret;

  for (auto f : boost::filesystem::directory_iterator(path)) {
    if (!om::file::IsFolder(f) || f == "_orig") {
      continue;
    }

    ret.push_back(f.path().string());
    log_infos << "found user " << f.path().string();
  }

  return ret;
}

void om::gui::userChooserWidget::Done() { chooser_->done(1); }
