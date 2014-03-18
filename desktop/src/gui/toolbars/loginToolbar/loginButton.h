#pragma once
#include "precomp.h"

#include "events/listeners.h"

class LoginButton : public QPushButton,
                    public om::event::ConnectionEventListener {
  Q_OBJECT;

 public:
  explicit LoginButton(QWidget* parent);

  void ConnectionChangeEvent();

 public
Q_SLOTS:
  void onClicked();

 private:
  void setLabel();
};
