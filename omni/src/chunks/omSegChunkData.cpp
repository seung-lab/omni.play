#include "chunks/omSegChunkData.h"
#include "chunks/omSegChunkDataVisitors.hpp"

OmSegChunkData::OmSegChunkData(OmSegmentation* vol,
                               OmSegChunk* chunk,
                               const OmChunkCoord &coord)
    : vol_(vol)
    , chunk_(chunk)
    , coord_(coord)
    , loadedData_(false)
{}

OmRawDataPtrs& OmSegChunkData::getRawData()
{
    if(!loadedData_){
        rawData_ = vol_->VolData()->getChunkPtrRaw(coord_);
        loadedData_ = true;
    }
    return rawData_;
}

PooledTile32Ptr OmSegChunkData::ExtractDataSlice32bit(const ViewType plane,
                                                      const int offset)
{
    return boost::apply_visitor(ExtractDataSlice32bitVisitor(vol_, coord_,
                                                             plane, offset),
                                getRawData());
}

void OmSegChunkData::RefreshDirectDataValues(const bool computeSizes,
                                             OmSegments* segments)
{
    boost::apply_visitor(RefreshDirectDataValuesVisitor(chunk_,
                                                        computeSizes,
                                                        segments),
                         getRawData());
}

void OmSegChunkData::RefreshBoundingData(OmSegments* segments)
{
    boost::apply_visitor(RefreshBoundingDataVisitor(chunk_, segments),
                         getRawData());
}

uint32_t OmSegChunkData::SetVoxelValue(const DataCoord& voxel, const uint32_t val)
{
    return boost::apply_visitor(SetVoxelValueVisitor(voxel, val),
                                getRawData());
}

uint32_t OmSegChunkData::GetVoxelValue(const DataCoord& voxel)
{
    return boost::apply_visitor(GetVoxelValueVisitor(voxel),
                                getRawData());
}

void OmSegChunkData::RewriteChunk(const boost::unordered_map<uint32_t, uint32_t>& vals)
{
    boost::apply_visitor(RewriteChunkVisitor(vol_, chunk_, vals),
                         getRawData());
}

om::shared_ptr<uint32_t> OmSegChunkData::GetCopyOfChunkDataAsUint32()
{
    return boost::apply_visitor(GetOmImage32ChunkVisitor(vol_, chunk_),
                                getRawData());
}
