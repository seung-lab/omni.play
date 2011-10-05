#pragma once

#include "common/omCommon.h"
#include <boost/tuple/tuple.hpp>
#include <boost/tuple/tuple_comparison.hpp>

class mipVolume;
class OmViewGroupState;

// Level, Coordinate, mipVolume*, freshness, OmViewGroupState*,
//  ViewType, OmSegmentColorCacheType
typedef boost::tuple<int, om::globalCoord,
                     mipVolume*, uint32_t,
                     OmViewGroupState*, ViewType,
                     OmSegmentColorCacheType> OmTileCoordKey;

class OmTileCoord : public OmTileCoordKey {
public:
    OmTileCoord();

    OmTileCoord(const int, const om::globalCoord&,
                mipVolume*, const uint32_t,
                OmViewGroupState*, const ViewType,
                const ObjectType);

    OmTileCoord(const int, const om::globalCoord&,
                mipVolume*, const uint32_t,
                OmViewGroupState*, const ViewType,
                const OmSegmentColorCacheType);

    ObjectType getVolType() const;
    om::chunkCoord getChunkCoord() const;

    inline int getLevel() const {
        return this->get<0>();
    }
    inline const om::globalCoord& getCoord() const {
        return this->get<1>();
    }
    inline mipVolume* getVolume() const {
        return this->get<2>();
    }
    inline uint32_t getFreshness() const {
        return this->get<3>();
    }
    inline OmViewGroupState* getViewGroupState() const {
        return this->get<4>();
    }
    inline ViewType getViewType() const {
        return this->get<5>();
    }
    inline OmSegmentColorCacheType getSegmentColorCacheType() const {
        return this->get<6>();
    }

    friend std::ostream& operator<<(std::ostream &, const OmTileCoord &);
};

