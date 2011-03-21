#ifndef OM_SLICE_CACHE_HPP
#define OM_SLICE_CACHE_HPP

#include "system/cache/omVolSliceCache.hpp"
#include "chunks/omRawChunkMemMapped.hpp"

#include <boost/tuple/tuple.hpp>
#include <boost/tuple/tuple_comparison.hpp>

/**
 * unmanaged cache of slices to speed-up brush select tool
 *
 * NOT thred-safe
 *
 * for transient-use only
 *
 * assumes slices are in MIP 0, and are all in the same plane
 *
 */

class OmSliceCacheBase {
public:
    virtual boost::shared_ptr<boost::unordered_set<OmSegID> >
    GetSegIDs(const std::map<OmChunkCoord, std::set<Vector3i> >& ptsInChunks,
              const int depth) = 0;

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

    boost::shared_ptr<boost::unordered_set<OmSegID> >
    GetSegIDs(const std::map<OmChunkCoord, std::set<Vector3i> >& ptsInChunks,
              const int depth)
    {
        boost::shared_ptr<boost::unordered_set<OmSegID> > ret =
            boost::make_shared<boost::unordered_set<OmSegID> >();

        FOR_EACH(iter, ptsInChunks)
        {
            boost::shared_ptr<T> slicePtr = getSlice(iter->first, depth);
            T const*const sliceData = slicePtr.get();

            FOR_EACH(vec, iter->second)
            {
                const Vector2i loc =
                    OmView2dConverters::Get2PtsInPlane(*vec, viewType_);

                OmSegID segID = 0;
                if(YZ_VIEW == viewType_){
                    segID = sliceData[chunkDim_ * loc.x + loc.y];
                } else {
                    segID = sliceData[chunkDim_ * loc.y + loc.x];
                }

                if(segID){
                    ret->insert(segID);
                }
            }
        }

        return ret;
    }

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

private:
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

    boost::shared_ptr<boost::unordered_set<OmSegID> >
    GetSegIDs(const std::map<OmChunkCoord, std::set<Vector3i> >& ptsInChunks,
              const int depth)
    {
        return cache_->GetSegIDs(ptsInChunks, depth);
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
