#pragma once

#include "system/omConnect.hpp"
#include <QtGui>

class OmCheckBoxWidget : public QCheckBox {
  Q_OBJECT;
  ;
 public:
  OmCheckBoxWidget(QWidget* p, const QString& title) : QCheckBox(title, p) {
    om::connect(this, SIGNAL(stateChanged(int)), this, SLOT(action(int)));
  }

 protected:
  void set(const bool val) {
    if (val) {
      setCheckState(Qt::Checked);

    } else {
      setCheckState(Qt::Unchecked);
    }
  }

 private
Q_SLOTS:
  void action(int state) {
    if (state == Qt::Checked) {
      doAction(true);

    } else {
      doAction(false);
    }
  }

 private:
  virtual void doAction(const bool isChecked) = 0;
};
