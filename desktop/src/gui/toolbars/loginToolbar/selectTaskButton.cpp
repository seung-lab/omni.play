#include "gui/toolbars/loginToolbar/selectTaskButton.h"
#include "system/omConnect.hpp"
#include "system/account.h"
#include "system/omAppState.hpp"

SelectTaskButton::SelectTaskButton(QWidget* parent)
    : QPushButton("Task", parent) {
  setStatusTip("Select A Task.");
  setFlat(true);
  // setShortcut(QKeySequence("Ctrl+A"));
  om::connect(this, SIGNAL(clicked()), this, SLOT(onClicked()));
  setEnabled(om::system::Account::IsLoggedIn());
}

void SelectTaskButton::onClicked() { OmAppState::OpenTaskSelector(); }

void SelectTaskButton::ConnectionChangeEvent() {
  setEnabled(om::system::Account::IsLoggedIn());
}