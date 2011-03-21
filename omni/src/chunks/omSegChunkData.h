#ifndef OM_SEG_CHUNK_DATA_H
#define OM_SEG_CHUNK_DATA_H

#include "chunks/omChunkCoord.h"
#include "chunks/omChunkTypes.hpp"
#include "volume/omVolumeTypes.hpp"

class OmSegments;
class OmSegmentation;

class OmSegChunkData {
public:
    OmSegChunkData(OmSegmentation* vol, OmSegChunk* chunk,
                   const OmChunkCoord &coord);

    /**
     * Analyze segmentation ImageData in the chunk associated to a MipCoord
     *  and store all values in the DataSegmentId set of the chunk.
     */
    //TODO: wrong comment--function has changed...
    void RefreshDirectDataValues(const bool, OmSegments* segments);

    void RefreshBoundingData(OmSegments* segments);

    boost::shared_ptr<uint32_t> ExtractDataSlice32bit(const ViewType, const int);
    boost::shared_ptr<uint32_t> GetCopyOfChunkDataAsUint32();

    void RewriteChunk(const boost::unordered_map<uint32_t, uint32_t>&);

    uint32_t SetVoxelValue(const DataCoord & voxel, const uint32_t val);
    uint32_t GetVoxelValue(const DataCoord & voxel);

private:
    OmSegmentation *const vol_;
    OmSegChunk *const chunk_;
    const OmChunkCoord coord_;

    bool loadedData_;

    OmRawDataPtrs rawData_;
    OmRawDataPtrs& getRawData();
};

#endif
