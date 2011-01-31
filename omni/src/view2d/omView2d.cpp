#include "view2d/omView2dEvents.hpp"
#include "view2d/omMouseEvents.hpp"
#include "view2d/omView2dZoom.hpp"
#include "view2d/omKeyEvents.hpp"
#include "view2d/omScreenPainter.hpp"
#include "view2d/omView2d.h"

OmView2d::OmView2d(const ViewType viewtype, QWidget* parent,
				   OmViewGroupState * vgs, OmMipVolume* vol,
				   const std::string& name)
	: OmView2dCore(parent, vol, vgs, viewtype, name)
	, complimentaryDock_(NULL)
	, mScreenShotSaver(new OmScreenShotSaver())
	, screenPainter_(new OmScreenPainter(this, state(), mScreenShotSaver.get()))
	, mouseEvents_(new OmMouseEvents(this, state()))
	, keyEvents_(new OmKeyEvents(this, state()))
	, events_(new OmView2dEvents(this, state()))
	, zoom_(new OmView2dZoom(state()))
{
	setFocusPolicy(Qt::ClickFocus);	// necessary for receiving keyboard events
	setMouseTracking(true);	// necessary for mouse-centered zooming
	setAutoFillBackground(false);	// necessary for proper QPainter functionality

	state()->ResetWindowState();
	OmCursors::setToolCursor(this);
	OmEvents::ViewCenterChanged();
}

OmView2d::~OmView2d()
{
	unlinkComplimentaryDock();
}

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

void OmView2d::resizeEvent(QResizeEvent * event)
{
	OmEvents::ViewCenterChanged();

	resizeGL(event->size());
	myUpdate();
}

void OmView2d::resizeGL(const QSize& size)
{
	state()->touchFreshnessAndRedraw2d();

	resetPbuffer(size);

	state()->setTotalViewport(size.width(), size.height());

	state()->SetViewSliceOnPan();
}

void OmView2d::paintEvent(QPaintEvent *){
	screenPainter_->FullRedraw2d();
}

void OmView2d::myUpdate()
{
	LineDrawer()->myUpdate();
	update();
}

QSize OmView2d::sizeHint () const {
	return OmStateManager::getViewBoxSizeHint();
}

void OmView2d::mousePressEvent(QMouseEvent * event)
{
	mouseEvents_->Press(event);
}

void OmView2d::doRedraw2d()
{
	state()->touchFreshnessAndRedraw2d();
	myUpdate();
}

void OmView2d::SetDepth(QMouseEvent * event)
{
	const ScreenCoord screenc = ScreenCoord(event->x(), event->y());
	const SpaceCoord newDepth = state()->ScreenToSpaceCoord(screenc);
	state()->setSliceDepth(newDepth);

	OmEvents::ViewCenterChanged();
}

void OmView2d::wheelEvent(QWheelEvent * event){
	mouseEvents_->Wheel(event);
}

void OmView2d::mouseMoveEvent(QMouseEvent * event){
	mouseEvents_->Move(event);
}

void OmView2d::mouseReleaseEvent(QMouseEvent * event){
	mouseEvents_->Release(event);
}

void OmView2d::keyPressEvent(QKeyEvent * event)
{
	if(!keyEvents_->Press(event)){
		OmView2dKeyPressEventListener::keyPressEvent(event);
		QWidget::keyPressEvent(event);
	}
}

void OmView2d::resetWindow()
{
	state()->ResetWindowState();
	OmEvents::ViewCenterChanged();
	doRedraw2d();
}
