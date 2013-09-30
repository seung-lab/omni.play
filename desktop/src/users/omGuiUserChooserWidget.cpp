#include "project/omProject.h"
#include "project/omProjectGlobals.h"
#include "users/omGuiUserChooser.h"
#include "users/omGuiUserChooserWidget.h"
#include "users/omGuiUserChooserWidgetButtons.hpp"
#include "users/omUsers.h"

#include <QVBoxLayout>
#include <QLabel>

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
  const QString folder =
      QString::fromStdString(OmProject::Globals().Users().UsersRootFolder());

  QDir dir(folder);

  const QStringList dirNames = dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);

  std::vector<std::string> ret;

  FOR_EACH(iter, dirNames) {
    const std::string str = iter->toStdString();

    if ("_orig" == str) {
      continue;
    }

    ret.push_back(str);
    std::cout << "found user " << str << "\n";
  }

  return ret;
}

void om::gui::userChooserWidget::Done() { chooser_->done(1); }
