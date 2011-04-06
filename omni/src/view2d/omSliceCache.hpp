#ifndef OM_SLICE_CACHE_HPP
#define OM_SLICE_CACHE_HPP

#include "chunks/omRawChunkMemMapped.hpp"
#include "system/cache/omVolSliceCache.hpp"
#include "view2d/brush/omChunksAndPts.hpp"
#include "utility/omSmartPtr.hpp"

#include <boost/tuple/tuple.hpp>
#include <boost/tuple/tuple_comparison.hpp>

/**
 * unmanaged cache of slices to speed-up brush select tool
 *
 * NOT thread-safe
 *
 * for transient-use only
 *
 * assumes slices are in MIP 0, and are all in the same plane
 *
 */

class OmSliceCacheBase {
public:
    virtual boost::shared_ptr<uint32_t> GetSlice(const OmChunkCoord& chunkCoord, const int depth) = 0;

    virtual OmSegID GetVoxelValue(const OmChunkCoord& chunkCoord,
                                  const Vector3i& chunkPos) = 0;
};

template <typename T>
class OmSliceCacheImpl : public OmSliceCacheBase {
private:
    OmSegmentation *const vol_;
    const ViewType viewType_;
    const int chunkDim_;

    // ignore mip level--always 0
    // x, y, z, depth
    typedef boost::tuple<int, int, int, int> OmSliceKey;

    std::map<OmSliceKey, boost::shared_ptr<T> > cache_;

public:
    OmSliceCacheImpl(OmSegmentation* vol, const ViewType viewType)
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

        boost::shared_ptr<T> slicePtr = getCacheSlice(key, chunkCoord, depth);

        T const*const sliceData = slicePtr.get();

        const uint32_t offset = chunkDim_*chunkPos.y + chunkPos.x;

        return sliceData[offset];
    }

    boost::shared_ptr<uint32_t> GetSlice(const OmChunkCoord& chunkCoord, const int depth)
    {
        return recast<uint32_t>(getSlice(chunkCoord, depth));
    }

private:
    template <typename U, typename>
    boost::shared_ptr<U> recast(boost::shared_ptr<uint32_t> ptr){
        return ptr;
    }

    template <typename RET, typename X>
    boost::shared_ptr<RET> recast(boost::shared_ptr<X> ptr)
    {
        const uint32_t numElements = chunkDim_*chunkDim_;

        boost::shared_ptr<RET> ret = OmSmartPtr<RET>::MallocNumElements(numElements, om::DONT_ZERO_FILL);

        std::copy(ptr.get(), ptr.get() + numElements, ret.get());

        return ret;
    }

    boost::shared_ptr<T> getSlice(const OmChunkCoord& chunkCoord, const int depth)
    {
        const OmSliceKey key(chunkCoord.Coordinate.x,
                             chunkCoord.Coordinate.y,
                             chunkCoord.Coordinate.z,
                             depth);

        return getCacheSlice(key, chunkCoord, depth);
    }

    inline boost::shared_ptr<T> getCacheSlice(const OmSliceKey& key,
                                              const OmChunkCoord& chunkCoord,
                                              const int depth)
    {
        if(cache_.count(key)){
            return cache_[key];
        }

        boost::shared_ptr<T> ret =
            vol_->SliceCache()->Get<T>(chunkCoord, depth, viewType_);

        if(ret){
            return ret;
        }

        ret = cache_[key] = loadSlice(chunkCoord, depth);

        vol_->SliceCache()->Set(chunkCoord, depth, viewType_, ret);

        return ret;
    }

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
    OmSegmentation *const vol_;
    const ViewType viewType_;
    const boost::scoped_ptr<OmSliceCacheBase> cache_;

public:
    OmSliceCache(OmSegmentation* vol, const ViewType viewType)
        : vol_(vol)
        , viewType_(viewType)
        , cache_(cacheFactory())
    {}

    inline OmSegID GetVoxelValue(const OmChunkCoord& chunkCoord,
                          const Vector3i& chunkPos)
    {
        return cache_->GetVoxelValue(chunkCoord, chunkPos);
    }

    boost::shared_ptr<uint32_t> GetSlice(const OmChunkCoord& chunkCoord, const int depth)
    {
        return cache_->GetSlice(chunkCoord, depth);
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
            return new OmSliceCacheImpl<float>(vol_, viewType_);
        case OmVolDataType::UNKNOWN:
        default:
            throw OmIoException("data type not handled");
        };
    }
};

#endif
