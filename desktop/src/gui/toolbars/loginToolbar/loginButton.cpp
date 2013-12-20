#include "gui/toolbars/loginToolbar/loginDialog.h"
#include "gui/toolbars/loginToolbar/loginButton.h"
#include "system/omConnect.hpp"
#include "system/account.h"
#include "system/omAppState.hpp"

LoginButton::LoginButton(QWidget* parent) : QPushButton("Login", parent) {
  setStatusTip("Login/Logout");
  setFlat(true);
  setShortcut(QKeySequence("Ctrl+A"));
  // QT5: connect(this, &LoginButton::clicked, this, &LoginButton::onClicked);
  om::connect(this, SIGNAL(clicked()), this, SLOT(onClicked()));
}

void LoginButton::onClicked() {
  LoginDialog dialog(this);
  if (dialog.exec()) {
    om::system::Account::set_endpoint(dialog.endpoint());
    auto res = om::system::Account::Login(dialog.username(), dialog.password());
    QString errorText;
    typedef om::system::Account::LoginResult LoginResult;
    switch (res) {
      case LoginResult::SUCCESS:
        setText(QString::fromStdString(om::system::Account::username()));
        OmAppState::OpenTaskSelector();
        return;
      case LoginResult::BAD_USERNAME_PW:
        errorText = tr("Incorrect Username or Password.");
        break;
      case LoginResult::CONNECTION_ERROR:
        errorText = tr("Server Error.");
        break;
      default:
        errorText = tr("Unknown Error.");
        break;
    }
    if (!om::system::Account::IsLoggedIn()) {
      setText("Login");
    }
    QMessageBox box(QMessageBox::Warning, "Error Logging In", errorText);
    box.exec();
    return;
  }
}

void LoginButton::ConnectionChangeEvent() { setText(tr("")); }