#include "system/omAppState.hpp"
#include "view2d/omKeyEvents.hpp"
#include "view2d/omMouseEvents.hpp"
#include "view2d/omView2d.h"
#include "view2d/omView2dEvents.hpp"
#include "view2d/omView2dZoom.hpp"
#include "system/omStateManager.h"
#include "gui/controls/inputContext.hpp"
#include "gui/controls/joiningSplittingInputContext.hpp"
#include "view2d/omMouseEventUtils.hpp"

OmView2d::OmView2d(const om::common::ViewType viewtype, QWidget* parent,
                   OmViewGroupState& vgs, OmMipVolume& vol,
                   const std::string& name)
    : OmView2dCore(parent, vol, vgs, viewtype, name),
      state_(State()),
      complimentaryDock_(nullptr),
      mouseEvents_(new OmMouseEvents(this, state_)),
      keyEvents_(new OmKeyEvents(this, state_)),
      events_(new OmView2dEvents(this, state_)),
      zoom_(new OmView2dZoom(*state_)),
      viewControls_(new ViewControls(this, &state_->getViewGroupState())) {
  setFocusPolicy(Qt::ClickFocus);  // necessary for receiving keyboard events
  setMouseTracking(true);          // necessary for mouse-centered zooming
  setAutoFillBackground(false);  // necessary for proper QPainter functionality

  state_->SetIntialWindowState();
  OmCursors::setToolCursor(this, OmStateManager::GetToolMode());
  om::event::ViewCenterChanged();
}

OmView2d::~OmView2d() { unlinkComplimentaryDock(); }

// disable control-tab switching between segmentation and channel windows
void OmView2d::unlinkComplimentaryDock() {
  if (!complimentaryDock_) {
    return;
  }

  QWidget* compWidget = complimentaryDock_->widget();
  if (compWidget) {
    OmView2d* v2d = static_cast<OmView2d*>(compWidget);
    v2d->SetComplimentaryDockWidget(nullptr);
  }
}

// have QT redraw the widget
void OmView2d::Redraw() { update(); }

// have QT redraw the widget
void OmView2d::RedrawBlocking() {
  blockingRedraw_ = true;
  update();
}

QSize OmView2d::sizeHint() const { return OmAppState::GetViewBoxSizeHint(); }

void OmView2d::mousePressEvent(QMouseEvent* event) {
  if (!viewControls_->mousePressEvent(event)) {
    mouseEvents_->Press(event);
    //event->ignore();
  }
}

void OmView2d::wheelEvent(QWheelEvent* event) {
  if (!viewControls_->wheelEvent(event)) {
    mouseEvents_->Wheel(event);
    //event->ignore();
  }
}

void OmView2d::mouseMoveEvent(QMouseEvent* event) {
  if (!viewControls_->mouseMoveEvent(event)) {
    mouseEvents_->Move(event);
    //event->ignore();
  }

void OmView2d::mouseReleaseEvent(QMouseEvent* event) {
  if (!viewControls_->mouseReleaseEvent(event)) {
    mouseEvents_->Release(event);
    //event->ignore();
  }
}

void OmView2d::keyPressEvent(QKeyEvent* event) {
  if (!viewControls_->keyPressEvent(event)) {
    keyEvents_->Press(event);
    if (!keyEvents_->Press(event)) {
      QWidget::keyPressEvent(event);
    }
    //event->ignore();
  }
}

void OmView2d::keyReleaseEvent(QKeyEvent* event) {
  if (!viewControls_->keyReleaseEvent(event)) {
    if (!keyEvents_->Release(event)) {
      QWidget::keyReleaseEvent(event);
    }
    //event->ignore();
  }
}


boost::optional<om::coords::Global> OmView2d::GetGlobalCoords(int x, int y) {
  om::coords::Screen clicked(x, y, state_->Coords());
  return clicked.ToGlobal();
}

boost::optional<SegmentDataWrapper> OmView2d::GetSelectedSegment(int x, int y) {
  boost::optional<SegmentDataWrapper> segmentDataWrapper;
  boost::optional<om::coords::Global> global = GetGlobalCoords(x, y);
  if (global) {
    segmentDataWrapper = om::mouse::event::getSelectedSegment(*state_, *global);
  }
  return segmentDataWrapper;
}

void OmView2d::ResetWidget() {
  state_->ResetWindowState();
  om::event::ViewCenterChanged();
  Redraw();
}

void OmView2d::enterEvent(QEvent*) {
  // steal focus when mouse hovers over widget
}

#ifndef ZI_OS_MACOS
void OmView2d::paintGL() {
  if (keyEvents_->IsBlockingKeyDown()) {
    blockingRedraw_ = true;
  }
  OmView2dCore::paintGL();
}
#endif
