#pragma once

#include <QtGui>

class OmWidget : public QWidget {
 public:
  OmWidget(QWidget* p) : QWidget(p) {}

  virtual QString getName() = 0;
};
