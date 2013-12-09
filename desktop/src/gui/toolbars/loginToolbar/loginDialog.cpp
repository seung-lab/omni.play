#include "gui/toolbars/loginToolbar/loginDialog.h"
#include "system/omLocalPreferences.hpp"

LoginDialog::LoginDialog(QWidget* parent) : QDialog(parent) {
  auto* buttons = new QDialogButtonBox(
      QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, this);

  auto* layout = new QFormLayout;
  setLayout(layout);
  editEndpoint_ = new QLineEdit(OmLocalPreferences::getEndpoint());
  editEndpoint_->setMinimumWidth(200);  // make it long enough. 200 in pixels.
  editUser_ = new QLineEdit(OmLocalPreferences::getUsername());
  // TODO: auto fill in user name, or have a user list
  editPassword_ = new QLineEdit();
  editPassword_->setEchoMode(QLineEdit::Password);

  layout->addRow("Endpoint:", editEndpoint_);
  layout->addRow("Username:", editUser_);
  layout->addRow("Password:", editPassword_);
  layout->addRow(buttons);
  if (editUser_->text().isEmpty()) {
    editUser_->setFocus();
  } else {
    editPassword_->setFocus();
  }
  connect(buttons, SIGNAL(accepted()), this, SLOT(accept()));
  connect(buttons, SIGNAL(rejected()), this, SLOT(reject()));
}

void LoginDialog::accept() {
  OmLocalPreferences::setEndpoint(editEndpoint_->text());
  OmLocalPreferences::setUsername(editUser_->text());
  QDialog::accept();
}
