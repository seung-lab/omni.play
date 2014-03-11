#pragma once

#include <QtGui>

class LoginButton;
class SelectTaskButton;

class LoginToolBar : public QToolBar {
  Q_OBJECT;

 public:
  explicit LoginToolBar(QWidget *parent = 0);

 private:
  LoginButton *const loginButton_;
  SelectTaskButton *const selectTaskButton_;
};
