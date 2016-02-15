#pragma once
#include "precomp.h"

#include "gui/widgets/omSegmentContextMenu.h"
#include "utility/segmentDataWrapper.hpp"

namespace om {
namespace landmarks {
class sdwAndPt;
}
}
class OmViewGroupState;

namespace om {
namespace v3d {

class View3d;
class MacOSXGestures;

class Ui {
 public:
  Ui(View3d& view3d, OmViewGroupState&);
  ~Ui();

  // ui events
  bool GestureEvent(QGestureEvent* event);

  void MousePressed(QMouseEvent* event);
  void MouseRelease(QMouseEvent* event);
  void MouseMove(QMouseEvent* event);
  void MouseDoubleClick(QMouseEvent* event);
  void MouseWheel(QWheelEvent* event);
  void KeyPress(QKeyEvent* event);

 private:
  View3d& view3d_;
  OmViewGroupState& vgs_;
  std::unique_ptr<MacOSXGestures> macGestures_;
  OmSegmentContextMenu segmentContextMenu_;
  SegmentDataWrapper prevSDW_;

  void navigationModeMousePressed(QMouseEvent* event);
  void navigationModeMouseRelease(QMouseEvent* event);
  void navigationModeMouseMove(QMouseEvent* event);
  void navigationModeMouseDoubleClick(QMouseEvent* event);
  void navigationModeMouseWheel(QWheelEvent* event);

  void joinSplitModeMouseReleased(om::tool::mode tool, QMouseEvent* event);
  void shatterModeMouseReleased(QMouseEvent* event);

  void cameraMovementMouseStart(QMouseEvent* event);
  void cameraMovementMouseEnd(QMouseEvent* event);
  void cameraMovementMouseUpdate(QMouseEvent* event);
  void cameraMovementMouseWheel(QWheelEvent* event);

  SegmentDataWrapper pickSegmentMouse(QMouseEvent* event, bool drag);

  void showSegmentContextMenu(QMouseEvent* event);

  void crosshair(QMouseEvent* event);
  void centerAxisOfRotation(QMouseEvent* event);
  om::landmarks::sdwAndPt pickVoxelMouseCrosshair(QMouseEvent* event);
  bool cutSegment(QMouseEvent* event);
  bool annotate(QMouseEvent* event);
  void resetWindow();

  void deselectSegment(const SegmentDataWrapper& sdw);
  void doSelectSegment(const SegmentDataWrapper& sdw, const bool);
};
}
}  // om::v3d::
