#pragma once

#include "segment/omSegments.h"

namespace om {
namespace segchunk {

class ProcessChunkVoxelBoundingData {
public:
    ProcessChunkVoxelBoundingData(OmSegChunk* chunk, OmSegments* segments)
        : chunk_(chunk)
        , minVertexOfChunk_(chunk_->Mipping().GetExtent().getMin())
        , segments_(segments)
    {}

    ~ProcessChunkVoxelBoundingData()
    {
        FOR_EACH(iter, cacheSegments_)
        {
            const OmSegID val = iter->first;
            OmSegment* seg = iter->second;
            seg->AddToBoundingBox(bounds_[val]);
        }
    }

    inline void processVoxel(const OmSegID val, const Vector3i& voxelPos)
    {
        getOrAddSegment(val);
        bounds_[val].merge(DataBbox(minVertexOfChunk_ + voxelPos,
                                    minVertexOfChunk_ + voxelPos));
    }

private:
    OmSegChunk *const chunk_;
    const Vector3i minVertexOfChunk_;
    OmSegments *const segments_;

    boost::unordered_map<OmSegID, OmSegment*> cacheSegments_;
    boost::unordered_map<OmSegID, DataBbox> bounds_;

    OmSegment* getOrAddSegment(const OmSegID val)
    {
        if(!cacheSegments_.count(val)){
            return cacheSegments_[val] = segments_->GetOrAddSegment(val);
        }
        return cacheSegments_[val];
    }
};

} // namespace segchunk
} // namespace om
