#ifndef OM_SEG_CHUNK_DATA_VISITORS_HPP
#define OM_SEG_CHUNK_DATA_VISITORS_HPP

#include "chunks/omRawChunkSlicer.hpp"
#include "chunks/omRawChunk.hpp"
#include "chunks/omSegChunk.h"
#include "segment/omSegments.h"
#include "system/cache/omVolSliceCache.hpp"
#include "utility/omChunkVoxelWalker.hpp"
#include "volume/build/omProcessSegmentationChunk.hpp"
#include "volume/io/omVolumeData.h"
#include "volume/omSegmentation.h"
#include "utility/image/omImage.hpp"

class ExtractDataSlice32bitVisitor
    : public boost::static_visitor<om::shared_ptr<uint32_t> >{
public:
    ExtractDataSlice32bitVisitor(OmSegmentation* vol, const OmChunkCoord& coord,
                                 const ViewType plane, int depth)
        : vol_(vol)
        , coord_(coord)
        , plane_(plane)
        , depth_(depth)
    {}

    template <typename T>
    inline om::shared_ptr<uint32_t> operator()(T* d) const
    {
        om::shared_ptr<T> dataPtr = getCachedRawSlice(d);
        OmImage<T, 2, OmImageRefData> slice(OmExtents[128][128], dataPtr.get());
        return slice.recastToUint32().getMallocCopyOfData();
    }

    inline om::shared_ptr<uint32_t> operator()(uint32_t* d) const {
        return getCachedRawSlice(d);
    }

    om::shared_ptr<uint32_t> operator()(float*) const {
        throw OmIoException("segmentation data shouldn't be float");
    }

private:
    template <typename T>
    inline om::shared_ptr<T> getCachedRawSlice(T* d) const
    {
        om::shared_ptr<T> dataPtr =
            vol_->SliceCache()->Get<T>(coord_, depth_, plane_);

        if(!dataPtr)
        {
            OmRawChunkSlicer<T> slicer(128, d);

            OmProject::Globals().FileReadSemaphore().acquire(1);
            dataPtr = slicer.GetCopyOfTile(plane_, depth_);
            OmProject::Globals().FileReadSemaphore().release(1);

            vol_->SliceCache()->Set(coord_, depth_, plane_, dataPtr);
        }

        return dataPtr;
    }

    OmSegmentation *const vol_;
    const OmChunkCoord coord_;
    const ViewType plane_;
    const int depth_;
};

class RefreshDirectDataValuesVisitor : public boost::static_visitor<>{
public:
    RefreshDirectDataValuesVisitor(OmSegChunk* chunk,
                                   const bool computeSizes,
                                   OmSegments* segments)
        : chunk_(chunk)
        , computeSizes_(computeSizes)
        , segments_(segments) {}

    template <typename T>
    void operator()(T* d) const{
        doRefreshDirectDataValues(d);
    }

private:
    OmSegChunk *const chunk_;
    const bool computeSizes_;
    OmSegments* segments_;

    template <typename C>
    void doRefreshDirectDataValues(C* data) const
    {
        OmProcessSegmentationChunk p(chunk_, computeSizes_, segments_);

        OmChunkVoxelWalker iter(128);
        for(iter.begin(); iter < iter.end(); ++iter){
            const OmSegID val = static_cast<OmSegID>(*data++);
            if(0 == val){
                continue;
            }
            p.processVoxel(val, *iter);
        }
    }
};

class ProcessChunkVoxelBoundingData {
public:
    ProcessChunkVoxelBoundingData(OmSegChunk* chunk,
                                  OmSegments* segments)
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

class RefreshBoundingDataVisitor : public boost::static_visitor<>{
public:
    RefreshBoundingDataVisitor(OmSegChunk* chunk,
                               OmSegments* segments)
        : chunk_(chunk)
        , segments_(segments) {}

    template <typename T>
    void operator()(T* d) const{
        doRefreshBoundingData(d);
    }
private:
    OmSegChunk *const chunk_;
    OmSegments* segments_;

    template <typename C>
    void doRefreshBoundingData(C* data) const
    {
        ProcessChunkVoxelBoundingData p(chunk_, segments_);

        OmChunkVoxelWalker iter(128);
        for(iter.begin(); iter < iter.end(); ++iter){
            const OmSegID val = static_cast<OmSegID>(*data++);
            if(0 == val){
                continue;
            }
            p.processVoxel(val, *iter);
        }
    }
};

class SetVoxelValueVisitor : public boost::static_visitor<uint32_t>{
public:
    SetVoxelValueVisitor(const DataCoord & voxel, uint32_t val)
        : voxel_(voxel), val_(val) {}

    template <typename T>
    uint32_t operator()(T* d) const
    {
        OmImage<T,3,OmImageRefData> data(OmExtents[128][128][128],
                                         d);
        const uint32_t oldVal = data.getVoxel(voxel_.z, voxel_.y, voxel_.x);
        data.setVoxel(voxel_.z, voxel_.y, voxel_.x, val_);
        return oldVal;
    }
private:
    const DataCoord & voxel_;
    uint32_t val_;
};

class GetVoxelValueVisitor : public boost::static_visitor<uint32_t>{
public:
    GetVoxelValueVisitor(const DataCoord & voxel)
        : voxel_(voxel) {}

    template <typename T>
    uint32_t operator()(T* d) const {
        OmImage<T,3,OmImageRefData> data(OmExtents[128][128][128],
                                         d);
        return data.getVoxel(voxel_.z, voxel_.y, voxel_.x);
    }
private:
    const DataCoord & voxel_;
};

class RewriteChunkVisitor : public boost::static_visitor<>{
private:
    OmMipVolume *const vol_;
    OmSegChunk *const chunk_;
    const boost::unordered_map<uint32_t, uint32_t>& vals_;

public:
    RewriteChunkVisitor(OmMipVolume* vol, OmSegChunk* chunk,
                        const boost::unordered_map<uint32_t, uint32_t>& vals)
        : vol_(vol)
        , chunk_(chunk)
        , vals_(vals)
    {}

    template <typename T>
    void operator()(T*) const
    {
        OmRawChunk<T> rawChunk(vol_, chunk_->GetCoordinate());
        rawChunk.SetDirty();
        T* d = rawChunk.Data();

        for(int i = 0; i < 128*128*128; ++i){
            d[i] = vals_.at(d[i]);
        }
    }
};

class GetOmImage32ChunkVisitor
    : public boost::static_visitor<om::shared_ptr<uint32_t> > {
public:
    GetOmImage32ChunkVisitor(OmMipVolume* vol, OmSegChunk* chunk)
        : vol_(vol)
        , chunk_(chunk)
        , numVoxelsInChunk_(chunk_->Mipping().NumVoxels())
    {}

    template <typename T>
    om::shared_ptr<uint32_t> operator()(T*) const
    {
        OmRawChunk<T> rawChunk(vol_, chunk_->GetCoordinate());

        om::shared_ptr<T> data = rawChunk.SharedPtr();
        T* dataRaw = data.get();

        om::shared_ptr<uint32_t> ret =
            OmSmartPtr<uint32_t>::MallocNumElements(numVoxelsInChunk_,
                                                    om::DONT_ZERO_FILL);
        std::copy(dataRaw,
                  dataRaw + numVoxelsInChunk_,
                  ret.get());

        return ret;
    }

    om::shared_ptr<uint32_t> operator()(uint32_t*) const
    {
        OmRawChunk<uint32_t> rawChunk(vol_, chunk_->GetCoordinate());
        return rawChunk.SharedPtr();
    }

    om::shared_ptr<uint32_t> operator()(float*) const {
        throw OmIoException("can't deal with float data!");
    }

private:
    OmMipVolume *const vol_;
    OmSegChunk *const chunk_;
    const int numVoxelsInChunk_;
};

#endif
