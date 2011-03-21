#include "system/omAppState.hpp"
#include "view2d/omView2dEvents.hpp"
#include "view2d/omMouseEvents.hpp"
#include "view2d/omView2dZoom.hpp"
#include "view2d/omKeyEvents.hpp"
#include "view2d/omScreenPainter.hpp"
#include "view2d/omView2d.h"

OmView2d::OmView2d(const ViewType viewtype, QWidget* parent,
                   OmViewGroupState* vgs, OmMipVolume* vol,
                   const std::string& name)
    : OmView2dCore(parent, vol, vgs, viewtype, name)
    , state_(State())
    , complimentaryDock_(NULL)
    , screenShotSaver_(new OmScreenShotSaver())
    , screenPainter_(new OmScreenPainter(this, state_, screenShotSaver_.get()))
    , mouseEvents_(new OmMouseEvents(this, state_))
    , keyEvents_(new OmKeyEvents(this, state_))
    , events_(new OmView2dEvents(this, state_))
    , zoom_(new OmView2dZoom(state_))
{
    setFocusPolicy(Qt::ClickFocus); // necessary for receiving keyboard events
    setMouseTracking(true); // necessary for mouse-centered zooming
    setAutoFillBackground(false); // necessary for proper QPainter functionality

    state_->ResetWindowState();
    OmCursors::setToolCursor(this);
    OmEvents::ViewCenterChanged();
}

OmView2d::~OmView2d()
{
    unlinkComplimentaryDock();
}

// disable control-tab switching between segmentation and channel windows
void OmView2d::unlinkComplimentaryDock()
{
    if(!complimentaryDock_){
        return;
    }

    QWidget* compWidget = complimentaryDock_->widget();
    if(compWidget){
        OmView2d* v2d = static_cast<OmView2d*>(compWidget);
        v2d->SetComplimentaryDockWidget(NULL);
    }
}

void OmView2d::resizeEvent(QResizeEvent* event)
{
    OmEvents::ViewCenterChanged();

    resizeGL(event->size());
    myUpdate();
}

void OmView2d::resizeGL(const QSize& size)
{
    state_->touchFreshnessAndRedraw2d();

    resetPbuffer(size);

    state_->setTotalViewport(size.width(), size.height());

    state_->SetViewSliceOnPan();
}

void OmView2d::paintEvent(QPaintEvent *){
    screenPainter_->FullRedraw2d();
}

void OmView2d::myUpdate(){
    update();
}

QSize OmView2d::sizeHint() const {
    return OmAppState::GetViewBoxSizeHint();
}

void OmView2d::mousePressEvent(QMouseEvent* event){
    mouseEvents_->Press(event);
}

void OmView2d::doRedraw2d()
{
    state_->touchFreshnessAndRedraw2d();
    myUpdate();
}

void OmView2d::wheelEvent(QWheelEvent* event){
    mouseEvents_->Wheel(event);
}

void OmView2d::mouseMoveEvent(QMouseEvent* event){
    mouseEvents_->Move(event);
}

void OmView2d::mouseReleaseEvent(QMouseEvent* event){
    mouseEvents_->Release(event);
}

void OmView2d::keyPressEvent(QKeyEvent* event)
{
    if(!keyEvents_->Press(event)){
        QWidget::keyPressEvent(event);
    }
}

void OmView2d::resetWindow()
{
    state_->ResetWindowState();
    OmEvents::ViewCenterChanged();
    doRedraw2d();
}

void OmView2d::enterEvent(QEvent*)
{
    // steal focus when mouse hovers over widget
}
