#pragma once

#include "common/common.h"
#include <boost/tuple/tuple.hpp>
#include <boost/tuple/tuple_comparison.hpp>
#include "coordinates/chunkCoord.h"
#include "segment/coloring.hpp"

class OmMipVolume;
class OmViewGroupState;

typedef boost::tuple<om::chunkCoord, om::common::ViewType, uint8_t,
                     OmMipVolume*, uint32_t, OmViewGroupState*,
                     om::segment::coloring> OmTileCoordKey;

class OmTileCoord : public OmTileCoordKey {
public:
    OmTileCoord();

    OmTileCoord(const om::chunkCoord&, om::common::ViewType, uint8_t,
                OmMipVolume*, uint32_t,
                OmViewGroupState*, om::segment::coloring);

    OmTileCoord(const om::chunkCoord&, om::common::ViewType, uint8_t,
                OmMipVolume*, uint32_t,
                OmViewGroupState*, om::common::ObjectType);

    inline const om::chunkCoord& getCoord() const {
        return this->get<0>();
    }
    inline om::common::ViewType getViewType() const {
        return this->get<1>();
    }
    inline uint8_t getDepth() const {
        return this->get<2>();
    }
    inline OmMipVolume* getVolume() const {
        return this->get<3>();
    }
    inline uint32_t getFreshness() const {
        return this->get<4>();
    }
    inline OmViewGroupState* getViewGroupState() const {
        return this->get<5>();
    }
    inline om::segment::coloring getSegmentColorCacheType() const {
        return this->get<6>();
    }

    OmTileCoord Downsample() const;

    friend std::ostream& operator<<(std::ostream &, const OmTileCoord &);
};

