#include "system/omAppState.hpp"
#include "view2d/omKeyEvents.hpp"
#include "view2d/omMouseEvents.hpp"
#include "view2d/omView2d.h"
#include "view2d/omView2dEvents.hpp"
#include "view2d/omView2dZoom.hpp"
#include "system/omStateManager.h"

OmView2d::OmView2d(const om::common::ViewType viewtype, QWidget* parent,
                   OmViewGroupState& vgs, OmMipVolume& vol,
                   const std::string& name)
    : OmView2dCore(parent, vol, vgs, viewtype, name),
      state_(State()),
      complimentaryDock_(nullptr),
      mouseEvents_(new OmMouseEvents(this, state_)),
      keyEvents_(new OmKeyEvents(this, state_)),
      events_(new OmView2dEvents(this, state_)),
      zoom_(new OmView2dZoom(*state_)) {
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
  mouseEvents_->Press(event);
}

void OmView2d::wheelEvent(QWheelEvent* event) { mouseEvents_->Wheel(event); }

void OmView2d::mouseMoveEvent(QMouseEvent* event) { mouseEvents_->Move(event); }

void OmView2d::mouseReleaseEvent(QMouseEvent* event) {
  mouseEvents_->Release(event);
}

void OmView2d::keyPressEvent(QKeyEvent* event) {
  if (!keyEvents_->Press(event)) {
    QWidget::keyPressEvent(event);
  }
}

void OmView2d::keyReleaseEvent(QKeyEvent* event) {
  if (!keyEvents_->Release(event)) {
    QWidget::keyReleaseEvent(event);
  }
}

std::unique_ptr<InputContext> getToolInputContext() {
  std::unique_ptr<InputContext> inputContext;
  om::tool::mode tool = OmStateManager::GetToolMode();
  switch (tool) {
    case om::tool::JOIN:
    case om::tool::SPLIT:
    case om::tool::MULTISPLIT:
      return std::make_unique<JoiningSplittingInputContext>(
          state_->GetViewGroupState(), tool,
          [=] (int x, int y) { return getSelectedSegment(x, y); });
    default:
      return inputContext;
  }
}

om::coords::Global getGlobalCoords(int x, int y) {
  om::coords::Screen clicked(event->x(), event->y(), state_->Coords());
  return clicked.ToGlobal();
}

std::shared_ptr<SegmentDataWrapper> getSelectedSegment(int x, int y) {
  return std::make_shared<SegmentDataWrapper>(
    new om::mouse::utils::getSelectedSegment(*state_, getGlobalCoords(x, y)));
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
