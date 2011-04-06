#ifndef OM_CHUNK_CACHE_HPP
#define OM_CHUNK_CACHE_HPP

#include "chunks/omChunkCoord.h"
#include "chunks/omChunkItemContainer.hpp"

/**
 * cache is a mutex-lock map of chunk coords to OmChunk objects
 **/

template <typename VOL, typename CHUNK>
class OmChunkCache {
private:
    boost::scoped_ptr<OmChunkItemContainer<VOL, CHUNK> > chunks_;

    void UpdateFromVolResize(){
        chunks_->UpdateFromVolResize();
    }

    friend class OmChannelImpl;
    friend class OmSegmentation;

public:
    OmChunkCache(VOL* vol)
        : chunks_(new OmChunkItemContainer<VOL, CHUNK>(vol))
    {}

    void GetChunk(boost::shared_ptr<CHUNK>& ptr, const OmChunkCoord& coord){
        chunks_->Get(ptr, coord);
    }
};

#endif
