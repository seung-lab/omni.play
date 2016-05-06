#pragma once
#include "precomp.h"

#include "common/common.h"
#include "gui/viewGroup/viewInputConversion.hpp"
#include "gui/controls/viewControls.hpp"

class SegmentDataWrapper;
class OmSegmentation;
class OmViewGroupState;

namespace om {
namespace prefs {
class View3d;
}

namespace v3d {

class Camera;
class Drawer;
class DrawStatus;
class OmniEventListener;
class Ui;
class Widgets;

class View3d : public QGLWidget, public ViewInputConversion {
  Q_OBJECT;

 public:
  View3d(QWidget*, OmViewGroupState&);
  ~View3d();

  Camera& GetCamera();
  void TimedUpdate();

  DrawStatus Status();

  const std::vector<OmSegmentation*>& Segmentations() { return segmentations_; }

  // gl actions
  virtual boost::optional<SegmentDataWrapper>
    GetSelectedSegment(int x, int y) override;
  virtual boost::optional<om::coords::Global>
    GetGlobalCoords(int x, int y) override;

  void DoZoom(const int direction);
  void DrawChunkBoundaries();

  void UpdatePreferences();

 protected:
  // preferences and draw settings
  void setBackgroundColor();
  void setCameraPerspective();
  void updateEnabledWidgets();

  // gl events
  void initializeGL();
  void resizeGL(int width, int height);
  void paintGL();

  // mouse events
  void mousePressEvent(QMouseEvent*);
  void mouseReleaseEvent(QMouseEvent*);
  void mouseMoveEvent(QMouseEvent*);
  void mouseDoubleClickEvent(QMouseEvent*);
  void wheelEvent(QWheelEvent*);
  void keyPressEvent(QKeyEvent*);
  void keyReleaseEvent(QKeyEvent*);
  bool event(QEvent*);
  void pinchTriggered(QPinchGesture*);

  QSize sizeHint() const;

 private:
  // draw methods
  void initLights();
  bool pickPoint(const Vector2i& point, std::vector<uint32_t>& names);

  std::unique_ptr<prefs::View3d> prefs_;
  std::unique_ptr<Widgets> widgets_;
  std::unique_ptr<Ui> ui_;
  std::unique_ptr<DrawStatus> drawStatus_;
  std::unique_ptr<Camera> camera_;
  std::unique_ptr<OmniEventListener> omniEventListener_;
  std::unique_ptr<ViewControls> viewControls_;
  const std::vector<OmSegmentation*> segmentations_;
  OmViewGroupState& vgs_;
  std::unique_ptr<Drawer> drawer_;

  QTime elapsed_;
  QTimer drawTimer_;
};
}
}  // om::v3d::
