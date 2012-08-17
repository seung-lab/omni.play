#include "system/omOpenGLGarbageCollector.hpp"
#include "system/omStateManager.h"
#include "tiles/cache/omTileCache.h"
#include "view2d/omScreenPainter.hpp"
#include "view2d/omTileDrawer.hpp"
#include "view2d/omView2dCore.h"
#include "view2d/omView2dState.hpp"
#include "viewGroup/omViewGroupState.h"

OmView2dCore::OmView2dCore(QWidget* parent, OmMipVolume* vol,
                           OmViewGroupState * vgs, const ViewType viewType,
                           const std::string& name)
#ifdef ZI_OS_MACOS
    : QWidget(parent)
#else
    : QGLWidget(parent)
#endif
    , blockingRedraw_(false)
    , viewType_(viewType)
    , name_(name)
    , state_(new OmView2dState(vol, vgs, viewType, size(), name))
    , tileDrawer_(new OmTileDrawer(state_.get(), viewType))
    , screenPainter_(new OmScreenPainter(this, state_.get()))
{
#ifdef ZI_OS_MACOS
	state_->setTotalViewport(size());
	resetPbuffer(size());
#endif
}

OmView2dCore::~OmView2dCore()
{}

void OmView2dCore::setupMainGLpaintOp()
{
    const Vector4i& vp = state_->getTotalViewport();

    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glPushMatrix();

    {
        glMatrixMode(GL_PROJECTION);

        glLoadIdentity();

        glOrtho(vp.lowerLeftX, // left
                vp.width,      // right
                vp.height,     // bottom
                vp.lowerLeftY, // top
                -1,            // nearClip
                1              // farClip
            );

        glMatrixMode(GL_MODELVIEW);

        glLoadIdentity();

        glDisable(GL_DEPTH_TEST);

        glEnable(GL_TEXTURE_2D);

        glDisable(GL_BLEND);

        // continued...
    }
}

void OmView2dCore::teardownMainGLpaintOp()
{
    glDisable(GL_TEXTURE_2D);

    glPopMatrix();
}

int OmView2dCore::GetTileCount(){
    return tileDrawer_->GetTileCount();
}

int OmView2dCore::GetTileCountIncomplete(){
    return tileDrawer_->GetTileCountIncomplete();
}

bool OmView2dCore::IsDrawComplete(){
    return tileDrawer_->IsDrawComplete();
}

void OmView2dCore::dockVisibilityChanged(const bool visible){
    OmTileCache::WidgetVisibilityChanged(tileDrawer_.get(), visible);
}

#ifdef ZI_OS_MACOS
void OmView2dCore::paintEvent (QPaintEvent* event)
{
	buffer_->makeCurrent();
	doPaintGL();
	buffer_->doneCurrent();
	QImage view = buffer_->toImage();

	QPainter painter(this);
    painter.drawImage(QPoint(0, 0), view);

    doPaintOther();
}

void OmView2dCore::resizeEvent (QResizeEvent* event)
{
	resizeGL(event->size().width(), event->size().height());
    resetPbuffer(QSize(width, height));
}

void OmView2dCore::resetPbuffer(const QSize& size)
{
    buffer_.reset(new QGLPixelBuffer(size,
		QGLFormat::defaultFormat(),
        state_->getViewGroupState()->get3dContext()));
}

#else

void OmView2dCore::initializeGL(){
    state_->setTotalViewport(size());
}

void OmView2dCore::resizeGL(int width, int height)
{
	doResize(width, height);
}

void OmView2dCore::paintGL()
{
	doPaintGL();
	doPaintOther();
}

#endif

void OmView2dCore::doPaintGL()
{
    setupMainGLpaintOp();
    {
        tileDrawer_->FullRedraw2d(blockingRedraw_);
        blockingRedraw_ = false;
    }
    teardownMainGLpaintOp();
}

void OmView2dCore::doPaintOther()
{
	screenPainter_->PaintExtras();

    if(!IsDrawComplete() || state_->getScribbling()){
        OmTileCache::SetDrawerActive(tileDrawer_.get());

    } else {
        OmTileCache::SetDrawerDone(tileDrawer_.get());

        if(!OmTileCache::AreDrawersActive()){
            OmOpenGLGarbageCollector::CleanTextureIDs(QGLContext::currentContext());
        }
    }

    if(!IsDrawComplete()){
        OmEvents::Redraw2d();
    }
}

void OmView2dCore::doResize(int width, int height)
{
    OmEvents::ViewCenterChanged();

    state_->setTotalViewport(width, height);
    state_->SetViewSliceOnPan();
}
