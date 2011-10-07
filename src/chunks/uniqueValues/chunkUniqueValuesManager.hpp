#pragma once

#include "chunks/uniqueValues/chunkUniqueValuesPerThreshold.hpp"
#include "chunks/uniqueValues/thresholdsInChunk.hpp"
#include "chunks/chunkItemContainer.hpp"

class chunkUniqueValuesManager {
private:
    typedef chunkItemContainer<segmentation, thresholdsInChunk> cont_t;
    boost::scoped_ptr<cont_t> chunks_;

    void UpdateFromVolResize(){
        chunks_->UpdateFromVolResize();
    }

    friend class segmentation;

public:
    chunkUniqueValuesManager(segmentation* segmentation)
        : chunks_(new cont_t(segmentation))
    {}

    ChunkUniqueValues Values(const coords::chunkCoord& coord, const double threshold)
    {
        thresholdsInChunk* thresholdsInChunk = chunks_->Get(coord);
        return thresholdsInChunk->Get(threshold)->Values();
    }

    ChunkUniqueValues RereadChunk(const coords::chunkCoord& coord, const double threshold)
    {
        thresholdsInChunk* thresholdsInChunk = chunks_->Get(coord);
        return thresholdsInChunk->Get(threshold)->RereadChunk();
    }

    void Clear(){
        chunks_->Clear();
    }
};

