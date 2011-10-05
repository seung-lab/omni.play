#pragma once

#include "chunks/uniqueValues/omChunkUniqueValuesPerThreshold.hpp"
#include "chunks/uniqueValues/omThresholdsInChunk.hpp"
#include "chunks/omChunkItemContainer.hpp"

class OmChunkUniqueValuesManager {
private:
    typedef OmChunkItemContainer<segmentation, OmThresholdsInChunk> cont_t;
    boost::scoped_ptr<cont_t> chunks_;

    void UpdateFromVolResize(){
        chunks_->UpdateFromVolResize();
    }

    friend class segmentation;

public:
    OmChunkUniqueValuesManager(segmentation* segmentation)
        : chunks_(new cont_t(segmentation))
    {}

    ChunkUniqueValues Values(const om::chunkCoord& coord, const double threshold)
    {
        OmThresholdsInChunk* thresholdsInChunk = chunks_->Get(coord);
        return thresholdsInChunk->Get(threshold)->Values();
    }

    ChunkUniqueValues RereadChunk(const om::chunkCoord& coord, const double threshold)
    {
        OmThresholdsInChunk* thresholdsInChunk = chunks_->Get(coord);
        return thresholdsInChunk->Get(threshold)->RereadChunk();
    }

    void Clear(){
        chunks_->Clear();
    }
};

