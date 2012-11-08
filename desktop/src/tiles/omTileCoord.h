#pragma once

#include "common/common.h"
#include "common/enums.hpp"
#include <boost/tuple/tuple.hpp>
#include <boost/tuple/tuple_comparison.hpp>

class OmMipVolume;
class OmViewGroupState;

typedef boost::tuple<om::coords::Chunk, om::common::ViewType, uint8_t,
                     OmMipVolume*, uint32_t, OmViewGroupState*,
                     om::common::SegmentColorCacheType> OmTileCoordKey;

class OmTileCoord : public OmTileCoordKey {
public:
    OmTileCoord(const om::coords::Chunk&, om::common::ViewType, uint8_t,
                OmMipVolume*, uint32_t,
                OmViewGroupState*, om::common::SegmentColorCacheType);

    OmTileCoord(const om::coords::Chunk&, om::common::ViewType, uint8_t,
                OmMipVolume*, uint32_t,
                OmViewGroupState*, om::common::ObjectType);

    inline const om::coords::Chunk& getCoord() const {
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
    inline om::common::SegmentColorCacheType getSegmentColorCacheType() const {
        return this->get<6>();
    }

    OmTileCoord Downsample() const;

    friend std::ostream& operator<<(std::ostream &, const OmTileCoord &);
};

