#pragma once
#include "precomp.h"

#include "view2d/omView2dState.hpp"
#include "view2d/omView2dCore.h"
#include "system/omStateManager.h"
#include "gui/controls/inputContext.hpp"
#include "coordinates/coordinates.h"
#include "gui/viewGroup/viewInputConversion.hpp"
#include "gui/controls/viewControls.hpp"

class OmKeyEvents; class OmMouseEvents;
class OmView2dEvents;
class OmView2dZoom;
class SegmentDataWrapper;

class OmView2d : public OmView2dCore, public ViewInputConversion {
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

  virtual boost::optional<SegmentDataWrapper>
    GetSelectedSegment(int x, int y) override;
  virtual boost::optional<om::coords::Global>
    GetGlobalCoords(int x, int y) override;

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
  ViewControls *viewControls_;

  void unlinkComplimentaryDock();
};
