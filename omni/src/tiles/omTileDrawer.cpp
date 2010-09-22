#include "common/om.hpp"
#include "project/omProject.h"
#include "tiles/cache/omTileCache.h"
#include "system/viewGroup/omViewGroupState.h"
#include "view2d/omOnScreenTileCoords.h"
#include "view2d/omTextureID.h"
#include "tiles/omTile.h"
#include "tiles/omTileCoord.h"
#include "tiles/omTileDrawer.hpp"
#include "view2d/omView2dState.hpp"
#include "volume/omChannel.h"
#include "volume/omFilter2d.h"

#include <zi/utility>

OmTileDrawer::OmTileDrawer(boost::shared_ptr<OmView2dState> state,
			   const ViewType vt)
	: state_(state)
	, mViewType(vt)
	, mTileCount(0)
	, mTileCountIncomplete(0)
{
	state_->getCache()->RegisterDrawer(this);

#ifdef WIN32
	mGlBlendColorFunction = (GLCOLOR) wglGetProcAddress("glBlendColor");
#endif

}

OmTileDrawer::~OmTileDrawer()
{
	state_->getCache()->UnRegisterDrawer(this);
}

void OmTileDrawer::fullRedraw()
{
	reset();

	OmMipVolume* vol = state_->getVol();
	draw(vol);

	if(CHANNEL == vol->getVolumeType()) {
		OmChannel& chan = OmProject::GetChannel(vol->getID());
		foreach( OmId id, chan.GetValidFilterIds() ) {
			OmFilter2d &filter = chan.GetFilter(id);
			drawFromFilter(filter);
		}
	}

	if(isDrawComplete()){
		state_->getCache()->SetDrawerDone(this);
	}
}

void OmTileDrawer::reset()
{
	mTileToDraw.clear();
	mTileCount = 0;
	mTileCountIncomplete = 0;
}

void OmTileDrawer::draw(OmMipVolume* vol)
{
	determineWhichTilesToDraw(vol);
	drawTiles();
}

void OmTileDrawer::determineWhichTilesToDraw(OmMipVolume* vol)
{
	OmTileCoordsAndLocationsPtr tileCoordsAndLocations =
		getTileCoordsAndLocationsForCurrentScene(vol);

	mTileCount += tileCoordsAndLocations->size();

	FOR_EACH(tileCL, *tileCoordsAndLocations){

		OM::BlockingRead amBlocking = OM::NON_BLOCKING;
		if(state_->getScribbling()){
			amBlocking = OM::BLOCKING;
		}

		OmTilePtr tile;
		state_->getCache()->Get(this, tile, tileCL->tileCoord,
					amBlocking);

		if(!tile){
			++mTileCountIncomplete;
			continue;
		}

		OmTileAndVertices tv = {tile, tileCL->vertices};
		mTileToDraw.push_back(tv);
	}
}

OmTileCoordsAndLocationsPtr
OmTileDrawer::getTileCoordsAndLocationsForCurrentScene(OmMipVolume* vol)
{
	OmMipVolume* curVol = state_->getVol();
	state_->setVol(vol);

	OmOnScreenTileCoords stc(state_);
	OmTileCoordsAndLocationsPtr ret = stc.ComputeCoordsAndLocations();

	state_->setVol(curVol);

	return ret;
}

void OmTileDrawer::drawTiles()
{
	FOR_EACH(it, mTileToDraw){
		drawTile(*it);
	}
}

void OmTileDrawer::drawTile(const OmTileAndVertices& tv)
{
	if(mViewType == YZ_VIEW) {
		glMatrixMode(GL_TEXTURE);
		glLoadIdentity();
		glTranslatef(0.5, 0.5, 0.0);
		glRotatef(-90, 0.0, 0.0, 1.0);
		glTranslatef(-0.5, -0.5, 0.0);
		glMatrixMode(GL_MODELVIEW);
	}

	const OmTextureIDPtr& texture = tv.tile->getTexture();
	bindTileDataToGLid(texture);
	glBindTexture(GL_TEXTURE_2D, texture->getTextureID());

	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f);	/* lower left corner */
	glVertex2f(tv.vertices.lowerLeft.x,
		   tv.vertices.lowerLeft.y);

	glTexCoord2f(1.0f, 0.0f);	/* lower right corner */
	glVertex2f(tv.vertices.lowerRight.x,
		   tv.vertices.lowerRight.y);

	glTexCoord2f(1.0f, 1.0f);	/* upper right corner */
	glVertex2f(tv.vertices.upperRight.x,
		   tv.vertices.upperRight.y);

	glTexCoord2f(0.0f, 1.0f);	/* upper left corner */
	glVertex2f(tv.vertices.upperLeft.x,
		   tv.vertices.upperLeft.y);
	glEnd();
}

void OmTileDrawer::bindTileDataToGLid(const OmTextureIDPtr& texture)
{
	if(texture->needToBuildTexture()){
		doBindTileDataToGLid(texture);
	}
}

void OmTileDrawer::doBindTileDataToGLid(const OmTextureIDPtr& texture)
{
	GLuint textureID;
	glGenTextures(1, &textureID);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

	const GLint format = texture->getGLformat();
	glTexImage2D(GL_TEXTURE_2D, 0,
		     format,
		     texture->getWidth(), texture->getHeight(),
		     0,
		     format,
		     GL_UNSIGNED_BYTE,
		     texture->getTileData());

	texture->textureBindComplete(textureID);
}

void OmTileDrawer::drawFromFilter(OmFilter2d& filter)
{
	if(!filter.setupVol()){
		return;
	}

	setupGLblendColor(filter.GetAlpha());
	{
		draw(filter.getVolume());
	}
	teardownGLblendColor();
}

void OmTileDrawer::setupGLblendColor(const float alpha)
{
	glEnable(GL_BLEND);	// enable blending for transparency
	glBlendFunc(GL_ONE_MINUS_CONSTANT_ALPHA, GL_CONSTANT_ALPHA);

#if WIN32
	mGlBlendColorFunction(1.f, 1.f, 1.f, (1.f - alpha));
#else
	glBlendColor(1.f, 1.f, 1.f, (1.f - alpha));
#endif

	// continued...
}

void OmTileDrawer::teardownGLblendColor(){
	glDisable(GL_BLEND);	//disable blending for transparency
}
