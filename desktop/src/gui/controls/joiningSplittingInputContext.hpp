#pragma once
#include "precomp.h"

#include "gui/controls/inputContext.hpp"
#include "gui/controls/viewInputContext.hpp"

/*
 * Implementation of input context that accepts controls for the tools
 * join and split
 */
class JoiningSplittingInputContext
: public InputContext,
  public ViewInputContext {
 public:
  JoiningSplittingInputContext(ViewInputConversion* viewInputConversion,
       OmViewGroupState* viewGroupState, om::tool::mode tool);

  virtual bool mousePressEvent(QMouseEvent* mouseEvent) override;
  virtual bool mouseMoveEvent(QMouseEvent* mouseEvent) override;
  virtual bool mouseReleaseEvent(QMouseEvent* mouseEvent) override;
  virtual bool keyReleaseEvent(QKeyEvent *keyEvent) override;

 private:
  om::tool::mode tool_;
};
