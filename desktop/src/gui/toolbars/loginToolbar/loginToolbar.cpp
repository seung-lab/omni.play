#include "gui/toolbars/loginToolbar/loginToolbar.h"
#include "gui/toolbars/loginToolbar/loginButton.h"

#include "gui/taskSelector/taskSelector.h"

LoginToolBar::LoginToolBar(QWidget* parent)
    : QToolBar("Account", parent), loginButton_(new LoginButton(this)) {

  addWidget(loginButton_);
}