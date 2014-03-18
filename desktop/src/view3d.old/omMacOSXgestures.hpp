#pragma once
#include "precomp.h"

#include "omView3d.h"

class OmMacOSXGestures {
 private:
  OmView3d *const view3d_;

 public:
  OmMacOSXGestures(OmView3d *view3d) : view3d_(view3d) {}

  bool GestureEvent(QGestureEvent *event) {
    if (QGesture *pinch = event->gesture(Qt::PinchGesture)) {
      pinchTriggered(static_cast<QPinchGesture *>(pinch));
    }
    if (QGesture *pan = event->gesture(Qt::PanGesture)) {
      panTriggered(static_cast<QPanGesture *>(pan));
    }
    if (QGesture *swipe = event->gesture(Qt::SwipeGesture)) {
      swipeTriggered(static_cast<QSwipeGesture *>(swipe));
    }

    event->accept();

    return true;
  }

 private:
  void pinchTriggered(QPinchGesture *gesture) {
    const QPinchGesture::ChangeFlags changeFlags = gesture->changeFlags();

    if (changeFlags & QPinchGesture::RotationAngleChanged) {
      // qreal value = gesture->property("rotationAngle").toReal();
      // qreal lastValue = gesture->property("lastRotationAngle").toReal();
      // rotationAngle += value - lastValue;
    }

    if (changeFlags & QPinchGesture::ScaleFactorChanged) {
      // qreal value = gesture->property("scaleFactor").toReal();
      // currentStepScaleFactor = value;
    }

    if (gesture->state() == Qt::GestureFinished) {
      qreal value = gesture->property("scaleFactor").toReal();
      // scaleFactor *= currentStepScaleFactor;
      // currentStepScaleFactor = 1;
      if (value < 1) {
        view3d_->DoZoom(-1);
      } else {
        view3d_->DoZoom(1);
      }
    }

    view3d_->update();
  }

  void panTriggered(QPanGesture *gesture) {
    QPointF delta = gesture->delta();
    view3d_->mCamera.MovementStart(CAMERA_PAN, Vector2f(0, 0));
    view3d_->mCamera.MovementUpdate(Vector2i(delta.x(), delta.y()));
    view3d_->mCamera.MovementEnd(Vector2i(delta.x(), delta.y()));

    view3d_->update();
  }

  void swipeTriggered(QSwipeGesture *gesture) {
    if (gesture->state() == Qt::GestureFinished) {
      if (gesture->horizontalDirection() == QSwipeGesture::Left ||
          gesture->verticalDirection() == QSwipeGesture::Up) {
        log_infos << "swipe 1";
        // goPrevImage();
      } else {
        log_infos << "swipe 2";
        // goNextImage();
      }
      log_infos << "swipe 3";
      view3d_->update();
    }
  }
};
