#pragma once
#include "precomp.h"

#include "common/common.h"
#include "coordinates/chunk.h"
#include "segment/coloring.hpp"

class OmMipVolume;
class OmViewGroupState;

typedef std::tuple<om::coords::Chunk, om::common::ViewType, uint8_t,
                   OmMipVolume*, uint32_t, OmViewGroupState*,
                   om::segment::coloring> OmTileCoordKey;

class OmTileCoord : public OmTileCoordKey {
 public:
  OmTileCoord(const om::coords::Chunk&, om::common::ViewType, uint8_t,
              OmMipVolume&, uint32_t, OmViewGroupState&, om::segment::coloring);

  OmTileCoord(const om::coords::Chunk&, om::common::ViewType, uint8_t,
              OmMipVolume&, uint32_t, OmViewGroupState&,
              om::common::ObjectType);

  inline const om::coords::Chunk& getCoord() const {
    return std::get<0>(*this);
  }
  inline om::common::ViewType getViewType() const { return std::get<1>(*this); }
  inline uint8_t getDepth() const { return std::get<2>(*this); }
  inline OmMipVolume& getVolume() const { return *std::get<3>(*this); }
  inline uint32_t getFreshness() const { return std::get<4>(*this); }
  inline OmViewGroupState& getViewGroupState() const {
    return *std::get<5>(*this);
  }
  inline om::segment::coloring getSegmentColorCacheType() const {
    return std::get<6>(*this);
  }

  OmTileCoord Downsample() const;

  friend std::ostream& operator<<(std::ostream&, const OmTileCoord&);
};
