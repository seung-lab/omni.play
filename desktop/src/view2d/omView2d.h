#pragma once
#include "precomp.h"

#include "view2d/omView2dState.hpp"
#include "view2d/omView2dCore.h"
#include "system/omStateManager.h"
#include "gui/controls/inputContext.hpp"
#include "coordinates/coordinates.h"

class OmKeyEvents; class OmMouseEvents;
class OmView2dEvents;
class OmView2dZoom;
class SegmentDataWrapper;

class OmView2d : public OmView2dCore {
  Q_OBJECT;

 public:
  OmView2d(const om::common::ViewType, QWidget *, OmViewGroupState &,
           OmMipVolume &, const std::string &name);
  ~OmView2d(); 
  void SetComplimentaryDockWidget(QDockWidget *dock) {
    complimentaryDock_ = dock;
  }

  void ShowComplimentaryDock() {
    if (complimentaryDock_) {
      complimentaryDock_->raise();
      complimentaryDock_->widget()->setFocus(Qt::OtherFocusReason);
    }
  }

  inline OmView2dZoom *Zoom() { return zoom_.get(); }

  void ResetWidget();

  void Redraw();
  void RedrawBlocking();

 protected:
  void keyPressEvent(QKeyEvent *event);
  void keyReleaseEvent(QKeyEvent *event);

  // mouse events
  void mouseMoveEvent(QMouseEvent *event);
  void mousePressEvent(QMouseEvent *event);
  void mouseReleaseEvent(QMouseEvent *event);
  void wheelEvent(QWheelEvent *event);
  void enterEvent(QEvent *);

  // Get the tool input context ( if any applicable)
  std::unique_ptr<InputContext> getToolInputContext();

  QSize sizeHint() const;
#ifndef ZI_OS_MACOS
  virtual void paintGL();
#endif

 private:
  OmView2dState *const state_;
  QDockWidget *complimentaryDock_;

  std::unique_ptr<OmMouseEvents> mouseEvents_;
  std::unique_ptr<OmKeyEvents> keyEvents_;
  std::unique_ptr<OmView2dEvents> events_;
  std::unique_ptr<OmView2dZoom> zoom_;
  ViewControls *viewControls;

  void unlinkComplimentaryDock();
  /*
   * Input: x, y coordinates of the screen
   * Returns: optional containing the clicked segment wrapped in
   *   a segmentDataWrapper if found
   */
  boost::optional<SegmentDataWrapper> getSelectedSegment(int x, int y);
  /*
   * Input: x, y coordinates of the screen
   * Returns: optional containing the coordinates in global space
   *   if found
   */
  boost::optional<om::coords::Global> getGlobalCoords(int x, int y);
};
