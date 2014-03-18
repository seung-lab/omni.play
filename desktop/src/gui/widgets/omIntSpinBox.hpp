#pragma once
#include "precomp.h"

#include "common/logging.h"
#include "events/events.h"
#include "system/omConnect.hpp"

class OmIntSpinBox : public QSpinBox {
  Q_OBJECT;

 public:
  OmIntSpinBox(QWidget* d, bool updateAsType) : QSpinBox(d) {
    if (updateAsType) {
      om::connect(this, SIGNAL(valueChanged(int)), this,
                  SLOT(spinboxChanged()));

    } else {
      om::connect(this, SIGNAL(editingFinished()), this,
                  SLOT(spinboxChanged()));
    }
  }

 private
Q_SLOTS:

  void spinboxChanged() {
    actUponValueChange(value());
    om::event::Redraw2d();
  }

 protected:
  virtual void setInitialGUIThresholdValue() {}
  virtual void actUponValueChange(const int value) = 0;
};
