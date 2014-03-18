#pragma once
#include "precomp.h"

class OmViewGroupState;

class BrushToolboxImpl : public QDialog {
 public:
  BrushToolboxImpl(QWidget* parent, OmViewGroupState* vgs);

  virtual ~BrushToolboxImpl() {}
};
