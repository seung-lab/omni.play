#pragma once

#include <QDialog>

class OmViewGroupState;

class BrushToolboxImpl : public QDialog {
 public:
  BrushToolboxImpl(QWidget* parent, OmViewGroupState* vgs);

  virtual ~BrushToolboxImpl() {}
};
