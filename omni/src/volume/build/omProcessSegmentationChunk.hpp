#ifndef OM_PROCESS_SEGMENTATION_CHUNK_HPP
#define OM_PROCESS_SEGMENTATION_CHUNK_HPP

#include "segment/omSegment.h"
#include "segment/omSegments.h"
#include "chunks/omChunk.h"
#include "chunks/omChunkCoord.h"

class OmProcessSegmentationChunk {
public:
    OmProcessSegmentationChunk(OmChunk* chunk,
                               const bool computeSizes,
                               OmSegments* segments)
        : chunk_(chunk)
        , computeSizes_(computeSizes)
        , minVertexOfChunk_(chunk_->Mipping().GetExtent().getMin())
        , segments_(segments)
    {}

    ~OmProcessSegmentationChunk()
    {
        FOR_EACH(iter, cacheSegments_)
        {
            const OmSegID val = iter->first;
            OmSegment* seg = iter->second;
            seg->addToSize(sizes_[val]);
            seg->AddToBoundingBox(bounds_[val]);
        }
    }

    inline void processVoxel(const OmSegID val, const Vector3i& voxelPos)
    {
        if(!computeSizes_){
            return;
        }

        getOrAddSegment(val);
        sizes_[val] = 1 + sizes_[val];
        bounds_[val].merge(DataBbox(minVertexOfChunk_ + voxelPos,
                                    minVertexOfChunk_ + voxelPos));
    }

private:
    OmChunk *const chunk_;
    const bool computeSizes_;
    const Vector3i minVertexOfChunk_;
    OmSegments *const segments_;

    boost::unordered_map<OmSegID, OmSegment*> cacheSegments_;
    boost::unordered_map<OmSegID, uint64_t> sizes_;
    boost::unordered_map<OmSegID, DataBbox> bounds_;

    OmSegment* getOrAddSegment(const OmSegID val)
    {
        if(!cacheSegments_.count(val)){
            return cacheSegments_[val] = segments_->GetOrAddSegment(val);
        }
        return cacheSegments_[val];
    }
};

#endif
