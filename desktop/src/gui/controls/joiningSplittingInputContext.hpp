#pragma once
#include "precomp.h"

#include "gui/controls/inputContext.hpp"
#include "gui/controls/viewStateInputContext.hpp"
#include "gui/controls/findSegment.hpp"

class JoiningSplittingInputContext
: public InputContext,
  public ViewStateInputContext,
  public FindSegment {
 public:
  JoiningSplittingInputContext(OmViewGroupState* viewGroupState,
      om::tool::mode tool,
      std::function<boost::optional<SegmentDataWrapper>(int, int)>
        findSegmentFunction);

  virtual bool mousePressEvent(QMouseEvent* mouseEvent) override;
  virtual bool mouseMoveEvent(QMouseEvent* mouseEvent) override;
  virtual bool mouseReleaseEvent(QMouseEvent* mouseEvent) override;
  virtual bool keyReleaseEvent(QKeyEvent *keyEvent) override;

 private:
  om::tool::mode tool_;
};
