#pragma once

#include "common/common.h"
#include <boost/tuple/tuple.hpp>
#include <boost/tuple/tuple_comparison.hpp>

class mipVolume;
class OmViewGroupState;

// Level, Coordinate, mipVolume*, freshness, OmViewGroupState*,
//  ViewType, segmentColorCacheType
typedef boost::tuple<int, coords::globalCoord,
                     mipVolume*, uint32_t,
                     OmViewGroupState*, ViewType,
                     segmentColorCacheType> tileCoordKey;

class tileCoord : public tileCoordKey {
public:
    tileCoord();

    tileCoord(const int, const coords::globalCoord&,
                mipVolume*, const uint32_t,
                OmViewGroupState*, const ViewType,
                const common::objectType);

    tileCoord(const int, const coords::globalCoord&,
                mipVolume*, const uint32_t,
                OmViewGroupState*, const ViewType,
                const segmentColorCacheType);

    common::objectType getVolType() const;
    coords::chunkCoord getChunkCoord() const;

    inline int getLevel() const {
        return this->get<0>();
    }
    inline const coords::globalCoord& getCoord() const {
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
    inline segmentColorCacheType getSegmentColorCacheType() const {
        return this->get<6>();
    }

    friend std::ostream& operator<<(std::ostream &, const tileCoord &);
};

