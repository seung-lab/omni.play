#ifndef OM_CHUNK_UNIQUE_VALUES_MANAGER_HPP
#define OM_CHUNK_UNIQUE_VALUES_MANAGER_HPP

#include "chunks/uniqueValues/omChunkUniqueValuesPerThreshold.hpp"
#include "chunks/uniqueValues/omThresholdsInChunk.hpp"
#include "chunks/omChunkItemContainer.hpp"

class OmChunkUniqueValuesManager {
private:
    typedef OmChunkItemContainer<OmSegmentation, OmThresholdsInChunk> cont_t;
    boost::scoped_ptr<cont_t> chunks_;

    void UpdateFromVolResize(){
        chunks_->UpdateFromVolResize();
    }

    friend class OmSegmentation;

public:
    OmChunkUniqueValuesManager(OmSegmentation* segmentation)
        : chunks_(new cont_t(segmentation))
    {}

    ChunkUniqueValues Values(const OmChunkCoord& coord, const double threshold)
    {
        OmThresholdsInChunk* thresholdsInChunk = chunks_->Get(coord);

        return thresholdsInChunk->Get(threshold)->Values();
    }

    void RereadChunk(const OmChunkCoord& coord, const double threshold)
    {
        OmThresholdsInChunk* thresholdsInChunk = chunks_->Get(coord);

        thresholdsInChunk->Get(threshold)->RereadChunk();
    }

    void Clear(){
        chunks_->Clear();
    }
};

#endif
