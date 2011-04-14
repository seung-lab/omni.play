#include "tiles/omPooledTile.hpp"
#include "common/omDebug.h"
#include "tiles/cache/omTileCache.h"
#include "system/omGarbage.h"
#include "tiles/omTextureID.h"

OmTextureID::OmTextureID(const int tileDim, OmPooledTile<uint8_t>* data)
    : tileDim_(tileDim)
    , pooledTile_(data)
    , flag_(OMTILE_NEEDTEXTUREBUILT)
{}

OmTextureID::OmTextureID(const int tileDim, OmPooledTile<OmColorARGB>* data)
    : tileDim_(tileDim)
    , pooledTile_(data)
    , flag_(OMTILE_NEEDCOLORMAP)
{}

OmTextureID::~OmTextureID()
{
    if(textureID_){
        OmGarbage::assignOmTextureId(*textureID_);
    }

    // data now in OpenGL texture; will be garbage collected via
    //  OmGarbage getting called from main GUI thread
}

void* OmTextureID::GetTileData() const
{
    if(!pooledTile_){
        throw OmIoException("no data");
    }

    return pooledTile_->GetDataVoid();
}

uchar* OmTextureID::GetTileDataUChar() const {
    return static_cast<uchar*>(GetTileData());
}

void OmTextureID::TextureBindComplete(const GLuint textureID)
{
    flag_ = OMTILE_GOOD;
    textureID_ = textureID;
    pooledTile_.reset();
}
