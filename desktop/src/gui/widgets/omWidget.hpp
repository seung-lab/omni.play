#pragma once
#include "precomp.h"

class OmWidget : public QWidget {
 public:
  OmWidget(QWidget* p) : QWidget(p) {}

  virtual QString getName() = 0;
};
