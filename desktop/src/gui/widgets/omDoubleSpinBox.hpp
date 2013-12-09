#pragma once

#include "common/logging.h"
#include "events/events.h"
#include "gui/widgets/omCursors.h"
#include "system/omConnect.hpp"

#include <QtGui>

class OmDoubleSpinBox : public QDoubleSpinBox {
  Q_OBJECT;

 public:
  OmDoubleSpinBox(QWidget* d, const bool updateAsType) : QDoubleSpinBox(d) {
    if (updateAsType) {
      om::connect(this, SIGNAL(valueChanged(double)), this,
                  SLOT(valueChanged()));

    } else {
      om::connect(this, SIGNAL(editingFinished()), this, SLOT(valueChanged()));
    }
  }

 private
Q_SLOTS:
  void valueChanged() {
    actUponValueChange(getGUIvalue());
    om::event::Redraw2d();
    om::event::Redraw3d();
  }

 protected:
  double getGUIvalue() { return value(); }

  void setGUIvalue(const double newThreshold) { setValue(newThreshold); }

  virtual void setInitialGUIThresholdValue() {}
  ;
  virtual void actUponValueChange(const double threshold) = 0;
};
