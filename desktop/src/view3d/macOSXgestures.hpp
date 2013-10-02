#pragma once

#include "view3d.h"

namespace om {
namespace v3d {

class MacOSXGestures {
 private:
  View3d &view3d_;

 public:
  MacOSXGestures(View3d &view3d) : view3d_(view3d) {}

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
        view3d_.DoZoom(-1);
      } else {
        view3d_.DoZoom(1);
      }
    }

    view3d_.update();
  }

  void panTriggered(QPanGesture *gesture) {
    QPointF delta = gesture->delta();
    // log_debug(unknown, "pan: %f, %f", delta.x(), delta.y());
    view3d_.GetCamera().MovementStart(CameraMovementType::PAN, Vector2f(0, 0));
    view3d_.GetCamera().MovementUpdate(Vector2i(delta.x(), delta.y()));
    view3d_.GetCamera().MovementEnd(Vector2i(delta.x(), delta.y()));

    view3d_.update();
  }

  void swipeTriggered(QSwipeGesture *gesture) {
    if (gesture->state() == Qt::GestureFinished) {
      if (gesture->horizontalDirection() == QSwipeGesture::Left ||
          gesture->verticalDirection() == QSwipeGesture::Up) {
        log_debugs(unknown) << "swipe 1";
        // goPrevImage();
      } else {
        log_debugs(unknown) << "swipe 2";
        // goNextImage();
      }
      log_debugs(unknown) << "swipe 3";
      view3d_.update();
    }
  }
};
}
}  // om::v3d::
