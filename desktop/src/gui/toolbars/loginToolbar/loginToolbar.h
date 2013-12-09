#pragma once

#include <QtGui>

class LoginButton;

class LoginToolBar : public QToolBar {
  Q_OBJECT;

 public:
  explicit LoginToolBar(QWidget *parent = 0);

 private:
  LoginButton *const loginButton_;
};
