#include "actions/omActions.h"
#include "events/events.h"
#include "landmarks/omLandmarks.hpp"
#include "mesh/omDrawOptions.h"
#include "project/omProject.h"
#include "segment/actions/omSplitSegmentRunner.hpp"
#include "segment/omSegmentCenter.hpp"
#include "segment/omSegmentSelected.hpp"
#include "segment/omSegmentSelector.h"
#include "segment/omSegmentUtils.hpp"
#include "segment/omSegments.h"
#include "system/omStateManager.h"
#include "omCamera.h"
#include "omMacOSXgestures.hpp"
#include "omSegmentPickPoint.h"
#include "omView3d.h"
#include "omView3dUi.h"
#include "viewGroup/omViewGroupState.h"
#include "volume/omSegmentation.h"
#include "annotation/annotation.h"

OmView3dUi::OmView3dUi(OmView3d* view3d, OmViewGroupState* vgs)
    : view3d_(view3d), vgs_(vgs), macGestures_(new OmMacOSXGestures(view3d)) {}

OmView3dUi::~OmView3dUi() {}

void OmView3dUi::MousePressed(QMouseEvent* event) {
  navigationModeMousePressed(event);
}

void OmView3dUi::MouseRelease(QMouseEvent* event) {
  navigationModeMouseRelease(event);
}

void OmView3dUi::MouseMove(QMouseEvent* event) {
  //ignore movement without buttons
  if (!event->buttons()) return;

  navigationModeMouseMove(event);
}

void OmView3dUi::MouseDoubleClick(QMouseEvent* event) {
  navigationModeMouseDoubleClick(event);
}

void OmView3dUi::MouseWheel(QWheelEvent* event) {
  navigationModeMouseWheel(event);
}

void OmView3dUi::KeyPress(QKeyEvent* event) {
  switch (event->key()) {
    case Qt::Key_C:
      OmSegmentCenter::CenterSegment(vgs_);
      break;
    case Qt::Key_Escape:
      resetWindow();
      break;
    case Qt::Key_Minus:
      view3d_->DoZoom(-1);
      break;
    case Qt::Key_Equal:
      view3d_->DoZoom(1);
      break;
  }
}

void OmView3dUi::splitModeMouseReleased(QMouseEvent* event) {
  const OmSegmentPickPoint pickPoint = pickVoxelMouseCrosshair(event);

  view3d_->updateGL();

  if (!pickPoint.sdw.IsSegmentValid()) {
    return;
  }

  OmSplitSegmentRunner::FindAndSplitSegments(pickPoint.sdw, vgs_,
                                             pickPoint.coord);
}

void OmView3dUi::shatterModeMouseReleased(QMouseEvent* event) {
  const OmSegmentPickPoint pickPoint = pickVoxelMouseCrosshair(event);

  view3d_->updateGL();

  if (!pickPoint.sdw.IsSegmentValid()) {
    return;
  }

  OmActions::ShatterSegment(pickPoint.sdw.GetSegment());
  vgs_->GetToolBarManager()->SetShatteringOff();
  OmStateManager::SetOldToolModeAndSendEvent();
}

bool OmView3dUi::cutSegment(QMouseEvent* event) {
  const SegmentDataWrapper sdw = pickSegmentMouse(event, false);

  view3d_->updateGL();

  if (!sdw.IsSegmentValid()) {
    return false;
  }

  OmActions::CutSegment(sdw);
  return true;
}

bool OmView3dUi::annotate(QMouseEvent* event) {
  const OmSegmentPickPoint pickPoint = pickVoxelMouseCrosshair(event);

  if (!pickPoint.sdw.IsSegmentValid()) {
    return false;
  }

  auto& manager = pickPoint.sdw.GetSegmentation().Annotations();

  manager.Add(pickPoint.coord, vgs_->getAnnotationString(),
              vgs_->getAnnotationColor(), vgs_->getAnnotationSize());
  return true;
}

/////////////////////////////////
///////          Navigation Mode Methods

void OmView3dUi::navigationModeMousePressed(QMouseEvent* event) {
  const bool leftMouseButton = event->buttons() & Qt::LeftButton;
  const bool rightMouseButton = event->buttons() & Qt::RightButton;
  const bool noModifiers = !event->modifiers();
  const bool controlModifier = event->modifiers() & Qt::ControlModifier;
  const bool altModifier = event->modifiers() & Qt::AltModifier;

  if (rightMouseButton) {
    if (noModifiers) {
      showSegmentContextMenu(event);

    } else if (controlModifier) {
      const bool shiftModifier = event->modifiers() & Qt::ShiftModifier;
      const OmSegmentPickPoint pickPoint = pickVoxelMouseCrosshair(event);
      doSelectSegment(pickPoint.sdw, shiftModifier);

    } else {
      cameraMovementMouseStart(event);
    }

    return;
  }

  if (leftMouseButton) {
    if (om::tool::LANDMARK == OmStateManager::GetToolMode()) {
      const OmSegmentPickPoint pickPoint = pickVoxelMouseCrosshair(event);
      vgs_->Landmarks().Add(pickPoint.sdw, pickPoint.coord);
      return;
    }

    if (om::tool::SPLIT == OmStateManager::GetToolMode()) {
      splitModeMouseReleased(event);
      return;
    }

    if (om::tool::SHATTER == OmStateManager::GetToolMode()) {
      shatterModeMouseReleased(event);
      return;
    }

    if (om::tool::CUT == OmStateManager::GetToolMode()) {
      if (cutSegment(event)) {
        return;
      }
    }

    if (om::tool::ANNOTATE == OmStateManager::GetToolMode()) {
      if (annotate(event)) {
        return;
      }
    }

    if (altModifier) {
      const OmSegmentPickPoint pickPoint = pickVoxelMouseCrosshair(event);
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

void OmView3dUi::deselectSegment(const SegmentDataWrapper& sdw) {
  if (!sdw.IsSegmentValid()) {
    return;
  }

  const om::common::ID segmentID = sdw.getID();

  OmSegmentSelector sel(sdw.MakeSegmentationDataWrapper(), this, "view3d");
  sel.augmentSelectedSet(segmentID, false);

  sel.sendEvent();
}

void OmView3dUi::doSelectSegment(const SegmentDataWrapper& sdw,
                                 const bool augment_selection) {
  if (!sdw.IsSegmentValid()) {
    return;
  }

  const om::common::ID segmentID = sdw.getID();

  OmSegmentSelected::Set(sdw);

  OmSegmentSelector sel(sdw.MakeSegmentationDataWrapper(), this, "view3d");

  if (augment_selection) {
    sel.augmentSelectedSet_toggle(segmentID);

  } else {
    sel.selectJustThisSegment_toggle(segmentID);
  }

  sel.sendEvent();
}

void OmView3dUi::navigationModeMouseRelease(QMouseEvent* event) {
  cameraMovementMouseEnd(event);
}

void OmView3dUi::navigationModeMouseMove(QMouseEvent* event) {
  cameraMovementMouseUpdate(event);
}

void OmView3dUi::navigationModeMouseDoubleClick(QMouseEvent* event) {
  centerAxisOfRotation(event);
}

void OmView3dUi::navigationModeMouseWheel(QWheelEvent* event) {
  cameraMovementMouseWheel(event);
}

/////////////////////////////////
///////          Camera Movement Methods

void OmView3dUi::cameraMovementMouseStart(QMouseEvent* event) {
  //get point and modifier
  Vector2f point = Vector2f(event->x(), event->y());
  bool shift_modifier = event->modifiers() & Qt::ShiftModifier;

  //left w/o shift moves rotate
  if (event->buttons() & Qt::LeftButton && !shift_modifier) {
    view3d_->mCamera.MovementStart(CAMERA_ORBIT_ROTATE, point);

    //left + shift moves lookat
  } else if (event->buttons() & Qt::LeftButton && shift_modifier) {
    view3d_->mCamera.MovementStart(CAMERA_LOOKAT_ROTATE, point);

    //mid button pans
  } else if (event->buttons() & Qt::MidButton) {
    view3d_->mCamera.MovementStart(CAMERA_PAN, point);

    //right button zooms
  } else if (event->buttons() & Qt::RightButton) {
    view3d_->mCamera.MovementStart(CAMERA_ZOOM, point);
  }
}

void OmView3dUi::cameraMovementMouseEnd(QMouseEvent* event) {
  Vector2f point = Vector2f(event->x(), event->y());
  view3d_->mCamera.MovementEnd(point);
}

void OmView3dUi::cameraMovementMouseUpdate(QMouseEvent* event) {
  Vector2f point = Vector2f(event->x(), event->y());
  view3d_->mCamera.MovementUpdate(point);
  view3d_->updateGL();
}

void OmView3dUi::cameraMovementMouseWheel(QWheelEvent* event) {
  Vector2f point = Vector2f(event->x(), event->y());
  view3d_->mCamera.MovementStart(CAMERA_ZOOM, point);

  // fake mouse drag
  // TODO: make mouse zoom speed a preference
  const int numDegrees = event->delta() / 8;
  const int numSteps = numDegrees / 15;
  if (numSteps >= 0) {
    point.y -= 30.f;
  } else {
    point.y += 30.f;
  }
  view3d_->mCamera.MovementUpdate(point);
  view3d_->updateGL();
  view3d_->mCamera.MovementEnd(point);
}

/////////////////////////////////
///////          Segment Picking

SegmentDataWrapper OmView3dUi::pickSegmentMouse(QMouseEvent* event,
                                                const bool drag) {
  //extract event properties
  Vector2i point2di(event->x(), event->y());

  const SegmentDataWrapper sdw = view3d_->PickPoint(point2di);
  if (!sdw.IsSegmentValid()) {
    return SegmentDataWrapper();
  }

  //check if dragging
  if (drag && sdw == prevSDW_) {
    return SegmentDataWrapper();
  } else {
    prevSDW_ = sdw;
  }

  return sdw;
}

/////////////////////////////////
///////           Segment Context Menu

void OmView3dUi::showSegmentContextMenu(QMouseEvent* event) {
  const OmSegmentPickPoint pickPoint = pickVoxelMouseCrosshair(event);

  view3d_->updateGL();

  if (!pickPoint.sdw.IsSegmentValid()) {
    return;
  }

  //refersh context menu and display
  segmentContextMenu_.Refresh(pickPoint, vgs_);
  segmentContextMenu_.exec(event->globalPos());
}

void OmView3dUi::centerAxisOfRotation(QMouseEvent* event) {
  const OmSegmentPickPoint pickPoint = pickVoxelMouseCrosshair(event);

  view3d_->updateGL();

  if (!pickPoint.sdw.IsSegmentValid()) {
    return;
  }

  view3d_->mCamera.SetFocus(pickPoint.coord);
  view3d_->updateGL();
}

void OmView3dUi::crosshair(QMouseEvent* event) {
  const OmSegmentPickPoint pickPoint = pickVoxelMouseCrosshair(event);

  view3d_->updateGL();

  if (!pickPoint.sdw.IsSegmentValid()) {
    return;
  }

  vgs_->View2dState()->SetScaledSliceDepth(pickPoint.coord);

  om::event::ViewCenterChanged();
}

OmSegmentPickPoint OmView3dUi::pickVoxelMouseCrosshair(QMouseEvent* event) {
  //extract event properties
  const Vector2i point2di(event->x(), event->y());

  view3d_->updateGL();

  const SegmentDataWrapper sdw = view3d_->PickPoint(point2di);
  if (!sdw.IsSegmentValid()) {
    return OmSegmentPickPoint();
  }

  Vector3f point3d;
  if (!view3d_->UnprojectPoint(point2di, point3d)) {
    return OmSegmentPickPoint();
  }

  OmSegmentPickPoint ret = { sdw, point3d };

  return ret;
}

void OmView3dUi::resetWindow() {
  static const Vector3f coord(0, 0, 0);

  view3d_->mCamera.SetFocus(coord);
  view3d_->mCamera.SetDistance(300.0);

  view3d_->updateGL();
}

bool OmView3dUi::GestureEvent(QGestureEvent* event) {
  return macGestures_->GestureEvent(event);
}
