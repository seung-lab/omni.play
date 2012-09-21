#pragma once

#include "volume/omMipVolume.h"
#include "system/cache/omGetSetCache.hpp"
#include "tiles/pools/omPooledTile.hpp"
#include "tiles/cache/raw/omRawSegTileCacheTypes.hpp"

#include <boost/tuple/tuple.hpp>
#include <boost/tuple/tuple_comparison.hpp>

// mip, x, y, z, depth, plane
typedef boost::tuple<int, int, int, int, int, ViewType> OmVolSliceKey_t;

struct OmVolSliceKey : public OmVolSliceKey_t {
    OmVolSliceKey()
        : OmVolSliceKey_t(-1, -1, -1, -1, -1, XY_VIEW)
    {}

    OmVolSliceKey(const om::chunkCoord& chunkCoord,
                  const int sliceDepth,
                  const ViewType viewType)
        : OmVolSliceKey_t(chunkCoord.Level,
                          chunkCoord.Coordinate.x,
                          chunkCoord.Coordinate.y,
                          chunkCoord.Coordinate.z,
                          sliceDepth,
                          viewType)
    {}
};

class OmRawSegTileCache {
private:
    OmMipVolume *const vol_;

    typedef OmGetSetCache<OmVolSliceKey, PooledTile32Ptr> cache_t;
    boost::scoped_ptr<cache_t> cache_;

public:
    OmRawSegTileCache(OmMipVolume* vol)
        : vol_(vol)
    {}

    void Load(){
        cache_.reset(new cache_t(om::TILE_CACHE, "slice cache",
                                 vol_->GetBytesPerSlice()));
    }

    void Clear(){
        cache_->Clear();
    }

    PooledTile32Ptr Get(const om::chunkCoord& chunkCoord, const int sliceDepth,
                        const ViewType viewType)
    {
        const OmVolSliceKey key(chunkCoord, sliceDepth, viewType);
        return cache_->Get(key);
    }

    void Set(const om::chunkCoord& chunkCoord, const int sliceDepth,
             const ViewType viewType, PooledTile32Ptr tile)
    {
        const OmVolSliceKey key(chunkCoord, sliceDepth, viewType);
        cache_->Set(key, tile);
    }
};

