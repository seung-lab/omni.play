#pragma once
#include "precomp.h"

/*
 * The tool mode is the input context for control.
 */
/*
 *class ToolControlContext {
 * public:
 *  // allow implementation cleanup
 *  virtual ~ToolControlContext();
 *
 *  // prevent copying
 *  ToolControlContext(ToolControlContext const &) = delete;
 *  ToolControlContext& operator=(ToolControlContext const &) = delete;
 *
 *  virtual mouseMoveEvent(QMouseEvent *event) const { ignore(); }
 *  virtual mousePressEvent(QMouseEvent *mouseEvent) const { ignore(); }
 *  virtual mouseReleaseEvent(QMouseEvent *mouseEvent) const { ignore(); }
 *  virtual mouseWheelEvent(QWheelEvent *wheelEvent) const { ignore(); }
 *  virtual keyPressEvent(QKeyEvent *keyEvent) const { ignore(); }
 *  virtual keyReleaseEvent(QKeyEvent *keyEvent) const { ignore(); }
 *
 * protected:
 *  ToolControlContext(OmViewGroupState viewGroupState,
 *      SegmentDataWrapper segmentDataWrapper, om::tool::mode tool)
 *    : viewGroupState_(viewGroupState), tool_(tool),
 *      segmentDataWrapper_(segmentDataWrapper) {
 *    }
 *
 * private:
 *  OmViewGroupState viewGroupState_;
 *  SegmentDataWrapper segmentDataWrapper_;
 *}
 */
