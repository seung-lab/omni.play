#pragma once

#include <QtGui>
#include "system/omConnect.hpp"

class OmLineEdit : public QLineEdit {
  Q_OBJECT;

 public:
  OmLineEdit(QWidget* parent) : QLineEdit(parent) {
    om::connect(this, SIGNAL(editingFinished()), this,
                SLOT(updateFromEditDoneSignel()));
  }

 private
Q_SLOTS:
  void updateFromEditDoneSignel() { doUpdateFromEditDoneSignel(); }

 private:
  virtual void doUpdateFromEditDoneSignel() = 0;

};
