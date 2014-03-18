#pragma once
#include "precomp.h"

#include "events/listeners.h"

class SelectTaskButton : public QPushButton,
                         public om::event::ConnectionEventListener {
  Q_OBJECT;

 public:
  explicit SelectTaskButton(QWidget* parent);

  void ConnectionChangeEvent() override;

 public
Q_SLOTS:
  void onClicked();
};
