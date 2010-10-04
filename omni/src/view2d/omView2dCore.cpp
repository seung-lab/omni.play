#include "system/omEvents.h"
#include "system/omGarbage.h"
#include "system/omStateManager.h"
#include "viewGroup/omViewGroupState.h"
#include "tiles/omTileDrawer.hpp"
#include "tiles/cache/omTileCache.h"
#include "view2d/omView2dCore.h"
#include "view2d/omView2dState.hpp"
#include "view2d/omLineDraw.hpp"

#include <boost/make_shared.hpp>

OmView2dCore::OmView2dCore(QWidget* parent, OmMipVolume* vol,
						   OmViewGroupState * vgs, const ViewType viewType,
						   const std::string& name)
	: QWidget(parent)
	, mViewType(viewType)
	, name_(name)
	, state_(boost::make_shared<OmView2dState>(vol, vgs, viewType, size(), name))
	, tileDrawer_(boost::make_shared<OmTileDrawer>(state_, viewType))
	, lineDraw_(boost::make_shared<OmLineDraw>(state_, viewType))
{
	resetPbuffer(size());
}

void OmView2dCore::resetPbuffer(const QSize& size)
{
	pbuffer_ = boost::make_shared<QGLPixelBuffer>(size,
												  QGLFormat::defaultFormat(),
												  (QGLWidget*)OmStateManager::GetPrimaryView3dWidget());
}

// pbuffered paint
QImage OmView2dCore::fullRedraw()
{
	reset();

	pbuffer_->makeCurrent();

	setupMainGLpaintOp();
	{
		OmGarbage::safeCleanTextureIds();

		tileDrawer_->fullRedraw();
	}
	teardownMainGLpaintOp();

	pbuffer_->doneCurrent();

	return pbuffer_->toImage();
}

void OmView2dCore::reset()
{
	state_->setTotalViewport(size());

	mNearClip = -1;
	mFarClip = 1;
}

void OmView2dCore::setupMainGLpaintOp()
{
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glPushMatrix(); {

		glMatrixMode(GL_PROJECTION);

		const Vector4i& vp = state_->getTotalViewport();

		glViewport(vp.lowerLeftX,
				   vp.lowerLeftY,
				   vp.width,
				   vp.height);
		glLoadIdentity();

		glOrtho(vp.lowerLeftX,	/* left */
				vp.width,	/* right */
				vp.height,	/* bottom */
				vp.lowerLeftY,	/* top */
				mNearClip,
				mFarClip);

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		glEnable(GL_TEXTURE_2D);

		glDisable(GL_BLEND);

		// continued...
	}
}

void OmView2dCore::teardownMainGLpaintOp()
{
	glPopMatrix();

	glDisable(GL_TEXTURE_2D);
}

int OmView2dCore::getTileCount(){
	return tileDrawer_->getTileCount();
}

int OmView2dCore::getTileCountIncomplete(){
	return tileDrawer_->getTileCountIncomplete();
}

bool OmView2dCore::isDrawComplete(){
	return tileDrawer_->isDrawComplete();
}

void OmView2dCore::dockVisibilityChanged(const bool visible)
{
	state_->getCache()->WidgetVisibilityChanged(tileDrawer_, visible);
}
