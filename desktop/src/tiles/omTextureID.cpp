#include "tiles/pools/omPooledTile.hpp"
#include "common/logging.h"
#include "tiles/cache/omTileCache.h"
#include "system/omOpenGLGarbageCollector.hpp"
#include "tiles/omTextureID.h"

OmTextureID::OmTextureID(const int tileDim, OmPooledTile<uint8_t>* data)
    : tileDim_(tileDim),
      pooledTile_(data),
      flag_(OMTILE_NEEDTEXTUREBUILT),
      context_(NULL) {}

OmTextureID::OmTextureID(const int tileDim,
                         OmPooledTile<om::common::ColorARGB>* data)
    : tileDim_(tileDim),
      pooledTile_(data),
      flag_(OMTILE_NEEDCOLORMAP),
      context_(NULL) {}

OmTextureID::~OmTextureID() {
  if (textureID_) {
    OmOpenGLGarbageCollector::AddTextureID(context_, *textureID_);
  }

  // data now in OpenGL texture; will be garbage collected via
  //  OmGarbage getting called from main GUI thread
}

void* OmTextureID::GetTileData() const {
  if (!pooledTile_) {
    throw om::IoException("no data");
  }

  return pooledTile_->GetDataVoid();
}

uchar* OmTextureID::GetTileDataUChar() const {
  return static_cast<uchar*>(GetTileData());
}

void OmTextureID::TextureBindComplete(QGLContext const* context,
                                      const GLuint textureID) {
  flag_ = OMTILE_GOOD;
  textureID_ = textureID;

  assert(context && "context should never be 0");
  context_ = context;
  pooledTile_.reset();
}
