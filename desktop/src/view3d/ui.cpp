#include "actions/omActions.h"
#include "annotation/annotation.h"
#include "events/events.h"
#include "gui/tools.hpp"
#include "landmarks/omLandmarks.hpp"
#include "landmarks/omLandmarksTypes.h"
#include "segment/omSegmentCenter.hpp"
#include "segment/omSegmentSelected.hpp"
#include "segment/omSegmentSelector.h"
#include "segment/omSegmentUtils.hpp"
#include "segment/selection.hpp"
#include "system/omStateManager.h"
#include "view3d/camera.h"
#include "view3d/macOSXgestures.hpp"
#include "view3d/ui.h"
#include "view3d/view3d.h"
#include "view3d/3dPreferences.hpp"
#include "viewGroup/omViewGroupState.h"
#include "viewGroup/omViewGroupView2dState.hpp"
#include "volume/omSegmentation.h"

namespace om {
namespace v3d {

Ui::Ui(View3d& view3d, OmViewGroupState& vgs)
    : view3d_(view3d), vgs_(vgs), macGestures_(new MacOSXGestures(view3d)) {}

Ui::~Ui() {}

void Ui::MousePressed(QMouseEvent* event) { navigationModeMousePressed(event); }

void Ui::MouseRelease(QMouseEvent* event) { navigationModeMouseRelease(event); }

void Ui::MouseMove(QMouseEvent* event) {
  // ignore movement without buttons
  if (!event->buttons()) return;
  navigationModeMouseMove(event);
}

void Ui::MouseDoubleClick(QMouseEvent* event) {
  navigationModeMouseDoubleClick(event);
}

void Ui::MouseWheel(QWheelEvent* event) { navigationModeMouseWheel(event); }

void Ui::KeyPress(QKeyEvent* event) {
  switch (event->key()) {
    case Qt::Key_C:
      OmSegmentCenter::CenterSegment(vgs_);
      break;
    case Qt::Key_Escape:
      resetWindow();
      break;
    case Qt::Key_Minus:
      view3d_.DoZoom(-1);
      break;
    case Qt::Key_Equal:
      view3d_.DoZoom(1);
      break;
  }
}

void Ui::KeyRelease(QKeyEvent* event) {
}

bool Ui::validateModeMouseReleased(om::common::SetValid setValid, QMouseEvent* event) {
  auto pickPoint = pickVoxelMouseCrosshair(event);

  if (!pickPoint.sdw.IsSegmentValid()
      || !pickPoint.sdw.Selection()->IsSegmentSelected(pickPoint.sdw.GetSegmentID())) {
    return false;
  }

  OmActions::ValidateSegment(pickPoint.sdw, setValid, true);
  return true;
}

bool Ui::shatterModeMouseReleased(QMouseEvent* event) {
  auto pickPoint = pickVoxelMouseCrosshair(event);

  if (!pickPoint.sdw.IsSegmentValid()) {
    return false;
  }

  OmActions::ShatterSegment(pickPoint.sdw.MakeSegmentationDataWrapper(),
                            pickPoint.sdw.GetSegment());
  OmStateManager::SetOldToolModeAndSendEvent();
  return true;
}

bool Ui::cutSegment(QMouseEvent* event) {
  boost::optional<SegmentDataWrapper> segmentDataWrapper =
    view3d_.FindSegment(event->x(), event->y());

  view3d_.updateGL();

  if (!segmentDataWrapper || !segmentDataWrapper->IsSegmentValid()) {
    return false;
  }

  OmActions::CutSegment(*segmentDataWrapper);
  return true;
}

bool Ui::annotate(QMouseEvent* event) {
  auto pickPoint = pickVoxelMouseCrosshair(event);

  if (!pickPoint.sdw.IsSegmentValid()) {
    return false;
  }

  auto& manager = pickPoint.sdw.GetSegmentation()->Annotations();

  manager.Add(pickPoint.coord, vgs_.getAnnotationString(),
              vgs_.getAnnotationColor(), vgs_.getAnnotationSize());

  return true;
}

/////////////////////////////////
///////          Navigation Mode Methods

void Ui::navigationModeMousePressed(QMouseEvent* event) {
  const auto buttons = event->buttons();
  const auto modifiers = event->modifiers();
  const bool leftMouseButton = buttons & Qt::LeftButton;
  const bool rightMouseButton = buttons & Qt::RightButton;
  const bool noModifiers = !modifiers;
  const bool controlModifier = modifiers & Qt::ControlModifier;
  const bool altModifier = modifiers & Qt::AltModifier;
  const bool shiftModifier = modifiers & Qt::ShiftModifier;

  if (rightMouseButton) {
    if (noModifiers) {
      showSegmentContextMenu(event);
    } else if (controlModifier) {
      auto pickPoint = pickVoxelMouseCrosshair(event);
      doSelectSegment(pickPoint.sdw, shiftModifier);
    } else {
      cameraMovementMouseStart(event);
    }
    return;
  }

  if (leftMouseButton) {
    const auto toolMode = OmStateManager::GetToolMode();
    switch (toolMode) {
      case om::tool::mode::LANDMARK:
        {
          auto pickPoint = pickVoxelMouseCrosshair(event);
          vgs_.Landmarks().Add(pickPoint.sdw, pickPoint.coord);
          return;
        }
      case om::tool::mode::VALIDATE:
        om::common::SetValid setValid;
        if (controlModifier) {
          setValid = om::common::SetValid::SET_NOT_VALID;
        } else {
          setValid = om::common::SetValid::SET_VALID;
        }
        if (validateModeMouseReleased(setValid, event)) {
          return;
        }
        break;
      case om::tool::mode::SHATTER:
        if (shatterModeMouseReleased(event)) {
          return;
        }
        break;
      case om::tool::mode::CUT:
        if (cutSegment(event)) {
          return;
        }
        break;
      case om::tool::mode::ANNOTATE:
        if (annotate(event)) {
          return;
        }
        break;
    }

    if (altModifier) {
      auto pickPoint = pickVoxelMouseCrosshair(event);
      deselectSegment(pickPoint.sdw);
      return;
    }

    if (controlModifier) {
      crosshair(event);
      return;
    }
  }
  cameraMovementMouseStart(event);
}

void Ui::deselectSegment(const SegmentDataWrapper& sdw) {
  if (!sdw.IsSegmentValid()) {
    return;
  }

  const om::common::ID segmentID = sdw.GetID();

  OmSegmentSelector sel(sdw.MakeSegmentationDataWrapper(), this, "view3d");
  sel.augmentSelectedSet(segmentID, false);
}

void Ui::doSelectSegment(const SegmentDataWrapper& sdw,
                         const bool augment_selection) {
  if (!sdw.IsSegmentValid()) {
    return;
  }

  const om::common::ID segmentID = sdw.GetID();

  OmSegmentSelected::Set(sdw);

  OmSegmentSelector sel(sdw.MakeSegmentationDataWrapper(), this, "view3d");

  if (augment_selection) {
    sel.augmentSelectedSet_toggle(segmentID);

  } else {
    sel.selectJustThisSegment_toggle(segmentID);
  }
}

void Ui::navigationModeMouseRelease(QMouseEvent* event) {
  const auto toolMode = OmStateManager::GetToolMode();
  cameraMovementMouseEnd(event);
}

void Ui::navigationModeMouseMove(QMouseEvent* event) {
  const auto modifiers = event->modifiers();
  const bool controlModifier = modifiers & Qt::ControlModifier; 
  const auto tool = OmStateManager::GetToolMode();

  if (cameraIsMoving()) {
    cameraMovementMouseUpdate(event);
    return;
  }
}

void Ui::navigationModeMouseDoubleClick(QMouseEvent* event) {
  centerAxisOfRotation(event);
}

void Ui::navigationModeMouseWheel(QWheelEvent* event) {
  cameraMovementMouseWheel(event);
}

/////////////////////////////////
///////          Camera Movement Methods

void Ui::cameraMovementMouseStart(QMouseEvent* event) {
  // get point and modifier
  Vector2f point = Vector2f(event->x(), event->y());
  bool shift_modifier = event->modifiers() & Qt::ShiftModifier;

  // left w/o shift moves rotate
  if (event->buttons() & Qt::LeftButton) {
    view3d_.GetCamera().MovementStart(CameraMovementType::ORBIT_ROTATE, point);

    // mid button pans
  } else if (event->buttons() & Qt::MidButton) {
    view3d_.GetCamera().MovementStart(CameraMovementType::PAN, point);

    // right + shift moves lookat
  } else if (event->buttons() & Qt::RightButton && shift_modifier) {
    view3d_.GetCamera().MovementStart(CameraMovementType::LOOKAT_ROTATE, point);

    // right button zooms
  } else if (event->buttons() & Qt::RightButton && !shift_modifier) {
    view3d_.GetCamera().MovementStart(CameraMovementType::ZOOM, point);
  }
}

void Ui::cameraMovementMouseEnd(QMouseEvent* event) {
  Vector2f point = Vector2f(event->x(), event->y());
  view3d_.GetCamera().MovementEnd(point);
}

void Ui::cameraMovementMouseUpdate(QMouseEvent* event) {
  Vector2f point = Vector2f(event->x(), event->y());
  view3d_.GetCamera().MovementUpdate(point);
  view3d_.updateGL();
}

bool Ui::cameraIsMoving() {
  return view3d_.GetCamera().IsMoving();
}

void Ui::cameraMovementMouseWheel(QWheelEvent* event) {
  bool altPressed = event->modifiers() & Qt::AltModifier;

  Vector2f point = Vector2f(event->x(), event->y());
  view3d_.GetCamera().MovementStart(CameraMovementType::ZOOM, point);

  // fake mouse drag
  const int numDegrees = event->delta() / 8;
  const int numSteps = numDegrees / 15;
  if (numSteps >= 0) {
    point.y -= altPressed ? 30.f : 30.f * Om3dPreferences::getScrollRate();
  } else {
    point.y += altPressed ? 30.f : 30.f * Om3dPreferences::getScrollRate();
  }
  view3d_.GetCamera().MovementUpdate(point);
  view3d_.updateGL();
  view3d_.GetCamera().MovementEnd(point);
}

/////////////////////////////////
///////           Segment Context Menu

void Ui::showSegmentContextMenu(QMouseEvent* event) {
  const auto pickPoint = pickVoxelMouseCrosshair(event);

  view3d_.updateGL();

  if (!pickPoint.sdw.IsSegmentValid()) {
    return;
  }

  // refersh context menu and display
  segmentContextMenu_.Refresh(pickPoint, vgs_);
  segmentContextMenu_.exec(event->globalPos());
}

void Ui::centerAxisOfRotation(QMouseEvent* event) {
  const auto pickPoint = pickVoxelMouseCrosshair(event);

  view3d_.updateGL();

  if (!pickPoint.sdw.IsSegmentValid()) {
    log_errors << "Invalid pickPoint " << pickPoint.sdw.GetSegmentationID();
    return;
  }

  view3d_.GetCamera().SetFocus(pickPoint.coord);
  view3d_.updateGL();
}

void Ui::crosshair(QMouseEvent* event) {
  const auto pickPoint = pickVoxelMouseCrosshair(event);

  view3d_.updateGL();

  if (!pickPoint.sdw.IsSegmentValid()) {
    return;
  }

  vgs_.View2dState().SetScaledSliceDepth(pickPoint.coord);

  om::event::ViewCenterChanged();
}

om::landmarks::sdwAndPt Ui::pickVoxelMouseCrosshair(QMouseEvent* event) {
  om::landmarks::sdwAndPt sdwAndPt;

  boost::optional<SegmentDataWrapper> segmentDataWrapper =
    view3d_.GetSelectedSegment(event->x(), event->y());
  if (!segmentDataWrapper || !segmentDataWrapper->IsSegmentValid()) {
    return sdwAndPt;
  }

  boost::optional<om::coords::Global> globalCoords;
  globalCoords = view3d_.FindGlobalCoords(event->x(), event->y());
  if (!globalCoords) {
    return sdwAndPt;
  }

  sdwAndPt = {*segmentDataWrapper, *globalCoords};

  return sdwAndPt;
}

void Ui::resetWindow() {
  static const Vector3f coord(0, 0, 0);

  view3d_.GetCamera().SetFocus(coord);
  view3d_.GetCamera().SetDistance(300.0);

  view3d_.updateGL();
}

bool Ui::GestureEvent(QGestureEvent* event) {
  return macGestures_->GestureEvent(event);
}
}
}  // om::v3d::
