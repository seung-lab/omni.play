#ifndef OM_SLICE_CACHE_HPP
#define OM_SLICE_CACHE_HPP

#include "chunks/omRawChunkMemMapped.hpp"

#include <boost/tuple/tuple.hpp>
#include <boost/tuple/tuple_comparison.hpp>

/**
 * lock-less, unmanaged cache of slices to speed-up brush select tool
 *
 * for transient-use only
 *
 * assumes slices are in MIP 0, and are all in the same plane
 *
 */

class OmSliceCacheBase {
public:
    virtual OmSegID GetVoxelValue(const OmChunkCoord& chunkCoord,
                                  const Vector3i& chunkPos) = 0;
};

template <typename T>
class OmSliceCacheImpl : public OmSliceCacheBase {
private:
    OmMipVolume *const vol_;
    const ViewType viewType_;
    const int chunkDim_;

    // ignore mip level--always 0
    // x, y, z, depth
    typedef boost::tuple<int, int, int, int> OmSliceKey;

    std::map<OmSliceKey, boost::shared_ptr<T> > cache_;

public:
    OmSliceCacheImpl(OmMipVolume* vol, const ViewType viewType)
        : vol_(vol)
        , viewType_(viewType)
        , chunkDim_(vol->Coords().GetChunkDimension())
    {}

    OmSegID GetVoxelValue(const OmChunkCoord& chunkCoord,
                          const Vector3i& chunkPos)
    {
        const int depth = chunkPos.z;

        const OmSliceKey key(chunkCoord.Coordinate.x,
                             chunkCoord.Coordinate.y,
                             chunkCoord.Coordinate.z,
                             depth);

        boost::shared_ptr<T> slicePtr;
        if(cache_.count(key)){
            slicePtr = cache_[key];
        } else {
            slicePtr = cache_[key] = loadSlice(chunkCoord, depth);
        }

        T const*const sliceData = slicePtr.get();

        const uint32_t offset = chunkDim_*chunkPos.y + chunkPos.x;

        return sliceData[offset];
    }

private:
    boost::shared_ptr<T> loadSlice(const OmChunkCoord& chunkCoord, const int depth)
    {
        OmRawChunkMemMapped<T> chunk(vol_, chunkCoord);
        return getSlice(chunk.Data(), depth);
    }

    boost::shared_ptr<T> getSlice(T* d, const int depth) const
    {
        OmImage<T, 3, OmImageRefData> chunk(OmExtents[chunkDim_][chunkDim_][chunkDim_], d);
        OmImage<T, 2> slice = chunk.getSlice(viewType_, depth);
        return slice.getMallocCopyOfData();
    }
};

class OmSliceCache {
private:
    OmMipVolume *const vol_;
    const ViewType viewType_;
    const boost::scoped_ptr<OmSliceCacheBase> cache_;

public:
    OmSliceCache(OmMipVolume* vol, const ViewType viewType)
        : vol_(vol)
        , viewType_(viewType)
        , cache_(cacheFactory())
    {}

    inline OmSegID GetVoxelValue(const OmChunkCoord& chunkCoord,
                          const Vector3i& chunkPos)
    {
        return cache_->GetVoxelValue(chunkCoord, chunkPos);
    }

private:
    OmSliceCacheBase* cacheFactory()
    {
        switch(vol_->getVolDataType().index()){
        case OmVolDataType::INT8:
            return new OmSliceCacheImpl<int8_t>(vol_, viewType_);
        case OmVolDataType::UINT8:
            return new OmSliceCacheImpl<uint8_t>(vol_, viewType_);
        case OmVolDataType::INT32:
            return new OmSliceCacheImpl<int32_t>(vol_, viewType_);
        case OmVolDataType::UINT32:
            return new OmSliceCacheImpl<uint32_t>(vol_, viewType_);
        case OmVolDataType::FLOAT:
        case OmVolDataType::UNKNOWN:
        default:
            throw OmIoException("data type not handled");
        };
    }
};

#endif
