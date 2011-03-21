#ifndef OM_CHUNK_CACHE_HPP
#define OM_CHUNK_CACHE_HPP

#include "zi/omMutex.h"
#include "chunks/omChunkCoord.h"

/**
 * cache is a mutex-lock map of chunk coords to OmChunk objects
 **/

template <typename VOL, typename CHUNK>
class OmChunkCache{
private:
    VOL *const vol_;;
    zi::mutex mutex_;
    std::map<OmChunkCoord, boost::shared_ptr<CHUNK> > map_;

public:
    OmChunkCache(VOL* vol)
        : vol_(vol)
    {}

    void Get(boost::shared_ptr<CHUNK>& ptr, const OmChunkCoord& coord)
    {
        zi::guard g(mutex_);
        if(map_.count(coord)){
            ptr = map_[coord];
        }else{
            assert(vol_->Coords().ContainsMipChunkCoord(coord));
            ptr = map_[coord] = boost::make_shared<CHUNK>(vol_, coord);
        }
    }
};

#endif
