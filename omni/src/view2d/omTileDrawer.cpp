#include "common/om.hpp"
#include "project/omProject.h"
#include "system/omGarbage.h"
#include "tiles/cache/omTileCache.h"
#include "tiles/omTextureID.h"
#include "tiles/omTextureID.h"
#include "tiles/omTile.h"
#include "tiles/omTileCoord.h"
#include "utility/dataWrappers.h"
#include "view2d/om2dPreferences.hpp"
#include "view2d/omOnScreenTileCoords.h"
#include "view2d/omTileDrawer.h"
#include "view2d/omView2dState.hpp"
#include "viewGroup/omViewGroupState.h"
#include "volume/omChannel.h"
#include "volume/omFilter2d.h"
#include "zi/omUtility.h"

OmTileDrawer::OmTileDrawer(boost::shared_ptr<OmView2dState> state,
                           const ViewType vt)
    : state_(state)
    , mViewType(vt)
    , mTileCount(0)
    , mTileCountIncomplete(0)
{
    OmTileCache::RegisterDrawer(this);

#ifdef WIN32
    mGlBlendColorFunction = (GLCOLOR) wglGetProcAddress("glBlendColor");
#endif
}

OmTileDrawer::~OmTileDrawer()
{
    OmTileCache::UnRegisterDrawer(this);
}

void OmTileDrawer::FullRedraw2d()
{
    reset();

    OmMipVolume* vol = state_->getVol();

    if(CHANNEL == vol->getVolumeType()) {
        ChannelDataWrapper cdw(vol->getID());

        const std::vector<OmFilter2d*> filters = cdw.GetFilters();

        FOR_EACH(iter, filters){
            filterDraw(*iter);
        }
    }

    draw(vol);

    if(IsDrawComplete()){
        OmTileCache::SetDrawerDone(this);
        if(!OmTileCache::AreDrawersActive()){
            OmGarbage::safeCleanTextureIds();
        }
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
    if(!vol->IsBuilt()){
        return;
    }

    determineWhichTilesToDraw(vol);
    drawTiles();
}

void OmTileDrawer::determineWhichTilesToDraw(OmMipVolume* vol)
{
    OmTileCoordsAndLocationsPtr tileCoordsAndLocations =
        getTileCoordsAndLocationsForCurrentScene(vol);

    mTileCount += tileCoordsAndLocations->size();

    om::Blocking amBlocking = om::NON_BLOCKING;
    if(state_->getScribbling()){
        amBlocking = om::BLOCKING;
    }

    FOR_EACH(tileCL, *tileCoordsAndLocations){

        OmTilePtr tile;
        OmTileCache::Get(this, tile, tileCL->tileCoord,
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

    const OmTextureIDPtr& texture = tv.tile->GetTexture();
    if(texture->NeedToBuildTexture()){
        doBindTileDataToGLid(texture);
    }

    glBindTexture(GL_TEXTURE_2D, texture->GetTextureID());

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

    const GLint format = texture->GetGLformat();
    glTexImage2D(GL_TEXTURE_2D, 0,
                 format,
                 texture->GetWidth(), texture->GetHeight(),
                 0,
                 format,
                 GL_UNSIGNED_BYTE,
                 texture->GetTileData());

    texture->TextureBindComplete(textureID);
}

void OmTileDrawer::setupGLblendColor(const float alpha,
                                     const bool shouldBrightenAlpha)
{
    glEnable(GL_BLEND);	// enable blending for transparency

    if(Om2dPreferences::HaveAlphaGoToBlack()){
        glBlendFunc(GL_CONSTANT_ALPHA, GL_ONE_MINUS_CONSTANT_ALPHA);
        glBlendColor(1.f, 1.f, 1.f, (1.f - alpha));

    } else {
        glBlendFuncSeparate(GL_SRC_COLOR, GL_CONSTANT_COLOR,
                            GL_ONE_MINUS_CONSTANT_ALPHA, GL_CONSTANT_ALPHA);

        float factor = 0.4;
        if(shouldBrightenAlpha){
            factor = 0.7;
        }
        const float val = alpha * factor;
        glBlendColor(val, val, val, val);
    }
}

bool OmTileDrawer::drawFromFilter(OmFilter2d* filter)
{
    if(!filter->HasValidVol()){
        return false;
    }

    OmMipVolume* vol = filter->GetMipVolume();
    draw(vol);

    if(CHANNEL == vol->getVolumeType()){
        return false;
    }

    const SegmentationDataWrapper& sdw = filter->GetSegmentationWrapper();
    return sdw.GetSelectedSegmentIds().size() == 0;
}

void OmTileDrawer::filterDraw(OmFilter2d* filter)
{
    const bool shouldBrightenAlpha = drawFromFilter(filter);
    setupGLblendColor(filter->GetAlpha(), shouldBrightenAlpha);
}
