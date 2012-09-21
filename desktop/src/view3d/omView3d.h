#pragma once

#include "omCamera.h"
#include "omView3dWidget.h"
#include "omView3dUi.h"

#include "events/details/omView3dEvent.h"
#include "events/details/omViewEvent.h"
#include "events/details/omSegmentEvent.h"
#include "events/details/omPreferenceEvent.h"
#include "common/omCommon.h"

#include <QGLWidget>
#include <QTimer>
#include <QtGui>

class OmViewGroupState;

class OmView3d : public QGLWidget,
                 public OmPreferenceEventListener,
                 public OmSegmentEventListener,
                 public OmView3dEventListener,
                 public OmViewEventListener
{
Q_OBJECT
public:
    OmView3d(QWidget *, OmViewGroupState * );
    ~OmView3d();

    OmCamera& GetCamera();

    const std::list<std::pair<float,float> >& PercVolDone() const {
        return percVolDone_;
    }

    inline bool MeshesFound() const {
        return meshesFound_;
    }

protected:
    //gl events
    void initializeGL();
    void resizeGL(int width, int height);
    void paintGL();

    //mouse events
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseDoubleClickEvent(QMouseEvent *event);
    void mouseWheelEvent(QWheelEvent * event);
    void wheelEvent(QWheelEvent * event );
    void keyPressEvent (QKeyEvent *);

    bool event(QEvent *e);
    void pinchTriggered(QPinchGesture *gesture);

    //omni events
    void SegmentModificationEvent(OmSegmentEvent *event);
    void SegmentGUIlistEvent(OmSegmentEvent*) {}
    void SegmentSelectedEvent(OmSegmentEvent*) {}
    void PreferenceChangeEvent(OmPreferenceEvent *event);
    void ViewBoxChangeEvent();
    void View3dRedrawEvent();
    void View3dRedrawEventFromCache();
    void ViewCenterChangeEvent() {}
    void View3dRecenter();
    void ViewPosChangeEvent() {}
    void ViewRedrawEvent() {}
    void ViewBlockingRedrawEvent() {}
    void AbsOffsetChangeEvent() {}

    //edit actions
    void SelectSegment(QMouseEvent *event);

    //voxel selection
    void FocusSelectVoxel(QKeyEvent *event);
    void MouseSelectVoxel(QMouseEvent *event, bool drag);
    void SelectVoxel(om::globalCoord &voxel, bool append);

    //gl actions
    SegmentDataWrapper PickPoint(const Vector2i& point2di);
    bool UnprojectPoint(Vector2i point2di, Vector3f &point3d);

    //draw methods
    void Draw(OmBitfield option);
    void DrawVolumes(OmBitfield option);
    void DrawWidgets();
    void doTimedDraw();
    void myUpdate();

    //draw settings
    void SetBackgroundColor();
    void SetBlending();
    void SetCameraPerspective();

    //widgets
    void UpdateEnabledWidgets();

    QSize sizeHint() const;

    void DoZoom(const int direction);

private:
    bool gestureEvent(QGestureEvent *event);
    OmView3dUi mView3dUi;
    OmViewGroupState* vgs_;
    boost::scoped_ptr<QTime> mElapsed;
    QTimer mDrawTimer;
    OmCamera mCamera;

    boost::ptr_vector<OmView3dWidget> widgets_;

    std::vector<int> mMousePickResult;

    void initLights();
    bool pickPoint(const Vector2i& point, std::vector<uint32_t>& names);

    std::list<std::pair<float,float> > percVolDone_;

    bool meshesFound_;

    const std::vector<OmSegmentation*> segmentations_;

    friend class OmMacOSXGestures;
    friend class OmView3dUi;
    friend class OmSelectionWidget;
    friend class OmChunkExtentWidget;
};

