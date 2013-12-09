
#include "common/logging.h"
#include "tiles/cache/omTileCache.h"
#include "system/omOpenGLGarbageCollector.hpp"
#include "tiles/omTextureID.h"

OmTextureID::OmTextureID(const int tileDim, std::shared_ptr<uint8_t> data)
    : tileDim_(tileDim),
      is8bit_(true),
      tile8_(data),
      flag_(OMTILE_NEEDTEXTUREBUILT),
      context_(nullptr) {}

OmTextureID::OmTextureID(const int tileDim,
                         std::shared_ptr<om::common::ColorARGB> data)
    : tileDim_(tileDim),
      is8bit_(false),
      tile32_(data),
      flag_(OMTILE_NEEDCOLORMAP),
      context_(nullptr) {}

OmTextureID::~OmTextureID() {
  if (textureID_) {
    OmOpenGLGarbageCollector::AddTextureID(context_, *textureID_);
  }

  // data now in OpenGL texture; will be garbage collected via
  //  OmGarbage getting called from main GUI thread
}

void* OmTextureID::GetTileData() const {
  if (!tile8_ && !tile32_) {
    throw om::IoException("no data");
  }

  if (is8bit_) {
    return static_cast<void*>(tile8_.get());
  }
  return static_cast<void*>(tile32_.get());
}

uint8_t* OmTextureID::GetTileDataUChar() const {
  return static_cast<uint8_t*>(GetTileData());
}

void OmTextureID::TextureBindComplete(QGLContext const* context,
                                      const GLuint textureID) {
  flag_ = OMTILE_GOOD;
  textureID_ = textureID;

  assert(context && "context should never be 0");
  context_ = context;
  tile8_.reset();
  tile32_.reset();
}
