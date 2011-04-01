#ifndef OM_MAC_OSX_GESTURES_HPP
#define OM_MAC_OSX_GESTURES_HPP

#include "omView3d.h"

class OmMacOSXGestures {
private:
    OmView3d *const view3d_;

public:
    OmMacOSXGestures(OmView3d* view3d)
        : view3d_(view3d)
    {}

    bool GestureEvent(QGestureEvent *event)
    {
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
        //printf("type %i\n", event->type());

        return true;
    }

private:

    void pinchTriggered(QPinchGesture *gesture)
    {
        const QPinchGesture::ChangeFlags changeFlags = gesture->changeFlags();

        if (changeFlags & QPinchGesture::RotationAngleChanged) {
            //printf("1\n");
            //qreal value = gesture->property("rotationAngle").toReal();
            //qreal lastValue = gesture->property("lastRotationAngle").toReal();
            //rotationAngle += value - lastValue;
        }

        if (changeFlags & QPinchGesture::ScaleFactorChanged) {
            //qreal value = gesture->property("scaleFactor").toReal();
            //printf("2 : %f\n", value);
            //currentStepScaleFactor = value;
        }

        if (gesture->state() == Qt::GestureFinished) {
            qreal value = gesture->property("scaleFactor").toReal();
            //printf("3 : %f\n", value);
            //scaleFactor *= currentStepScaleFactor;
            //currentStepScaleFactor = 1;
            if(value < 1) {
                view3d_->DoZoom(-1);
            } else {
                view3d_->DoZoom(1);
            }
        }

        view3d_->update();
    }

    void panTriggered(QPanGesture *gesture)
    {
        QPointF delta = gesture->delta();
        //printf("pan: %f, %f\n", delta.x(), delta.y());
        view3d_->mCamera.MovementStart(CAMERA_PAN, Vector2f(0,0));
        view3d_->mCamera.MovementUpdate(Vector2i(delta.x(),delta.y()));
        view3d_->mCamera.MovementEnd(Vector2i(delta.x(),delta.y()));

        view3d_->update();
    }

    void swipeTriggered(QSwipeGesture *gesture)
    {
        if (gesture->state() == Qt::GestureFinished) {
            if (gesture->horizontalDirection() == QSwipeGesture::Left
                || gesture->verticalDirection() == QSwipeGesture::Up) {
                printf("swipe 1\n");
                //goPrevImage();
            } else {
                printf("swipe 2\n");
                //goNextImage();
            }
            printf("swipe 3\n");
            view3d_->update();
        }
    }
};

#endif
