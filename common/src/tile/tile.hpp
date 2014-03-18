#pragma once

#include "precomp.h"
#include "common/common.h"
#include "coordinates/coordinates.h"

namespace om {
namespace tile {

template <typename T>
class Tile {
 public:
  Tile(coords::Tile coord, coords::VolumeSystem vol)
      : coord_(coord), vol_(vol) {
    Vector3i dims = common::twist(vol_.ChunkDimensions(), coord_.viewType());
    tileDims_.x = dims.x;
    tileDims_.y = dims.y;
    data_.reset(new T[tileDims_.x * tileDims_.y]);
  }

  Tile(coords::Tile coord, coords::VolumeSystem vol, std::shared_ptr<T> data)
      : coord_(coord), vol_(vol), data_(data) {
    Vector3i dims = common::twist(vol_.ChunkDimensions(), coord_.viewType());
    tileDims_.x = dims.x;
    tileDims_.y = dims.y;
  }

  const T& operator[](uint i) const {
    assert(i < tileDims_.x * tileDims_.y);
    return data_.get()[i];
  }

  T& operator[](uint i) {
    assert(i < tileDims_.x * tileDims_.y);
    return data_.get()[i];
  }

  std::shared_ptr<T> data() const { return data_; }

  inline size_t length() const { return tileDims_.x * tileDims_.y; }

  inline size_t size() const { return length() * sizeof(T); }

 private:
  PROP_CONST_REF(coords::Tile, coord);
  PROP_CONST_REF(coords::VolumeSystem, vol);
  PROP_CONST_REF(Vector2i, tileDims);
  std::shared_ptr<T> data_;
};

typedef boost::variant<Tile<int8_t>, Tile<uint8_t>, Tile<int32_t>,
                       Tile<uint32_t>, Tile<float>> TileVar;

GET_VAR_PROP(Tile, coords::Tile, coord);
GET_VAR_PROP(Tile, coords::VolumeSystem, vol);
GET_VAR_PROP(Tile, Vector2i, tileDims);
}
}  // namespace om::tile::
