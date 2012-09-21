#pragma once

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

    CHUNK* GetChunk(const om::chunkCoord& coord){
        return chunks_->Get(coord);
    }
};

