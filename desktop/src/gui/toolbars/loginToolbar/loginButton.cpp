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
  if (!dialog.exec()) {
    return;
  }
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
  auto request =
      om::system::Account::Login(dialog.username(), dialog.password());

  request >> [](om::system::LoginResult res) {
    QString errorText;
    switch (res) {
      case om::system::LoginResult::SUCCESS:
        om::event::ExecuteOnMain([]() { OmAppState::OpenTaskSelector(); });
        return;
      case om::system::LoginResult::BAD_USERNAME_PW:
        errorText = tr("Incorrect Username or Password.");
        break;
      case om::system::LoginResult::CONNECTION_ERROR:
        errorText = tr("Server Error.");
        break;
      default:
        errorText = tr("Unknown Error.");
        break;
    }
    om::event::NonFatalEventOccured(errorText);
  };
}

void LoginButton::ConnectionChangeEvent() { setLabel(); }

void LoginButton::setLabel() {
  if (om::system::Account::IsLoggedIn()) {
    setText(QString::fromStdString(om::system::Account::username()));
  } else {
    setText("Login");
  }
}