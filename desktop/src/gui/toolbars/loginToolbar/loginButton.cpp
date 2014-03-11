#include "gui/toolbars/loginToolbar/loginDialog.h"
#include "gui/toolbars/loginToolbar/loginButton.h"
#include "system/omConnect.hpp"
#include "system/account.h"
#include "system/omAppState.hpp"
#include "events/events.h"

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
    boost::optional<om::network::Uri> uri =
        om::network::Uri::Parse(dialog.endpoint());
    if (!uri) {
      QMessageBox box(QMessageBox::Warning, "Error Logging In",
                      "Invalid Endpoint");
      box.exec();
      return;
    }
    log_infos << "Connecting to: " << uri.get();
    om::system::Account::set_endpoint(uri.get());
    auto res = om::system::Account::Login(dialog.username(), dialog.password());
    QString errorText;
    typedef om::system::Account::LoginResult LoginResult;
    switch (res) {
      case LoginResult::SUCCESS:
        setLabel();
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
    setLabel();
    om::event::NonFatalEventOccured(errorText);
    return;
  }
}

void LoginButton::ConnectionChangeEvent() { setLabel(); }

void LoginButton::setLabel() {
  if (om::system::Account::IsLoggedIn()) {
    setText(QString::fromStdString(om::system::Account::username()));
  } else {
    setText("Login");
  }
}