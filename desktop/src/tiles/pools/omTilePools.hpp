#pragma once

#include "common/colors.h"
#include "tiles/pools/omTilePool.hpp"
#include "zi/omUtility.h"

class OmTilePools : private om::singletonBase<OmTilePools> {
 private:
  std::unique_ptr<OmTilePool<int8_t> > tilePoolInt8_;
  std::unique_ptr<OmTilePool<uint8_t> > tilePoolUint8_;
  std::unique_ptr<OmTilePool<int32_t> > tilePoolInt32_;
  std::unique_ptr<OmTilePool<uint32_t> > tilePoolUint32_;
  std::unique_ptr<OmTilePool<float> > tilePoolFloat_;
  std::unique_ptr<OmTilePool<om::common::ColorARGB> > tilePoolARGB_;

 public:
  template <typename T> static OmTilePool<T>& GetPool() {
    return instance().getTilePool(static_cast<T*>(0));
  }

  static void Reset() { instance().setup(); }

 private:
  inline OmTilePool<int8_t>& getTilePool(int8_t*) { return *tilePoolInt8_; }

  inline OmTilePool<uint8_t>& getTilePool(uint8_t*) { return *tilePoolUint8_; }

  inline OmTilePool<int32_t>& getTilePool(int32_t*) { return *tilePoolInt32_; }

  inline OmTilePool<uint32_t>& getTilePool(uint32_t*) {
    return *tilePoolUint32_;
  }

  inline OmTilePool<float>& getTilePool(float*) { return *tilePoolFloat_; }

  inline OmTilePool<om::common::ColorARGB>& getTilePool(
      om::common::ColorARGB*) {
    return *tilePoolARGB_;
  }

 private:
  OmTilePools() { setup(); }

  void setup() {
    tilePoolInt8_.reset(new OmTilePool<int8_t>(20, 128 * 128));
    tilePoolUint8_.reset(new OmTilePool<uint8_t>(20, 128 * 128));
    tilePoolInt32_.reset(new OmTilePool<int32_t>(20, 128 * 128));
    tilePoolUint32_.reset(new OmTilePool<uint32_t>(20, 128 * 128));
    tilePoolFloat_.reset(new OmTilePool<float>(20, 128 * 128));
    tilePoolARGB_.reset(new OmTilePool<om::common::ColorARGB>(20, 128 * 128));
  }

  friend class zi::singleton<OmTilePools>;
};
