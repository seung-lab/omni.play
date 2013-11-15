#pragma once

#include <QtGui>

class LoginDialog : public QDialog {
  Q_OBJECT;

 public:
  explicit LoginDialog(QWidget* parent = nullptr);

  std::string endpoint() { return editEndpoint_->text().toStdString(); }

  std::string username() { return editUser_->text().toStdString(); }

  std::string password() { return editPassword_->text().toStdString(); }

 public
Q_SLOTS:
  void accept() override;

 private:
  QLineEdit* editEndpoint_;
  QLineEdit* editUser_;
  QLineEdit* editPassword_;
};
