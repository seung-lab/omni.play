#include "tiles/omPooledTile.hpp"
#include "common/omDebug.h"
#include "tiles/cache/omTileCache.h"
#include "system/omGarbage.h"
#include "tiles/omTextureID.h"

OmTextureID::OmTextureID()
    : dims_(Vector2i(0,0))
    , flag_(OMTILE_COORDINVALID)
    , pooledTile_(NULL)
{}

OmTextureID::OmTextureID(const Vector2i& dims,
                         OmPooledTile<uint8_t>* data)
    : dims_(dims)
    , flag_(OMTILE_NEEDTEXTUREBUILT)
    , pooledTile_(data)
{}

OmTextureID::OmTextureID(const Vector2i& dims,
                         OmPooledTile<OmColorARGB>* data)
    : dims_(dims)
    , flag_(OMTILE_NEEDCOLORMAP)
    , pooledTile_(data)
{}

OmTextureID::~OmTextureID()
{
    deleteTileData();

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

void OmTextureID::deleteTileData()
{
    delete pooledTile_;
    pooledTile_ = NULL;
}
