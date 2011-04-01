#ifndef OM_VOL_SLICE_CACHE_HPP
#define OM_VOL_SLICE_CACHE_HPP

#include "volume/omMipVolume.h"
#include "datalayer/omDataWrapper.h"
#include "system/cache/omGetSetCache.hpp"

#include <boost/tuple/tuple.hpp>
#include <boost/tuple/tuple_comparison.hpp>

// mip, x, y, z, depth, plane
typedef boost::tuple<int, int, int, int, int, ViewType> OmVolSliceKey_t;

struct OmVolSliceKey : public OmVolSliceKey_t {
    OmVolSliceKey()
        : OmVolSliceKey_t(-1, -1, -1, -1, -1, XY_VIEW)
    {}

    OmVolSliceKey(const OmChunkCoord& chunkCoord,
                  const int sliceDepth,
                  const ViewType viewType)
        : OmVolSliceKey_t(chunkCoord.Level,
                          chunkCoord.Coordinate.x,
                          chunkCoord.Coordinate.y,
                          chunkCoord.Coordinate.z,
                          sliceDepth,
                          viewType)
    {}
};

class OmVolSliceCacheBase {
public:
    virtual void Clear() = 0;
    virtual OmDataWrapperPtr Get(const OmVolSliceKey&) = 0;
    virtual void Set(const OmVolSliceKey&, const OmDataWrapperPtr) = 0;
};

template <typename T>
class OmVolSliceCacheImpl : public OmVolSliceCacheBase {
private:
    typedef OmGetSetCache<OmVolSliceKey, boost::shared_ptr<T> > cache_t;

    boost::scoped_ptr<cache_t> cache_;

public:
    OmVolSliceCacheImpl(OmMipVolume* vol)
        : cache_(new cache_t(om::TILE_CACHE, "slice cache",
                             vol->GetBytesPerSlice()))
    {}

    virtual void Clear(){
        cache_->Clear();
    }

    virtual OmDataWrapperPtr Get(const OmVolSliceKey& key){
        return om::ptrs::Wrap(cache_->Get(key));
    }

    virtual void Set(const OmVolSliceKey& key, const OmDataWrapperPtr slice){
        cache_->Set(key, om::ptrs::UnWrap<T>(slice));
    }
};

class OmVolSliceCache {
private:
    OmMipVolume *const vol_;
    boost::scoped_ptr<OmVolSliceCacheBase> cache_;

public:
    OmVolSliceCache(OmMipVolume* vol)
        : vol_(vol)
    {}

    void Load(){
        cache_.reset(cacheFactory(vol_));
    }

    void Clear(){
        cache_->Clear();
    }

    template <typename T>
    boost::shared_ptr<T> Get(const OmChunkCoord& chunkCoord,
                             const int sliceDepth,
                             const ViewType viewType)
    {
        const OmVolSliceKey key(chunkCoord, sliceDepth, viewType);
        return om::ptrs::UnWrap<T>(cache_->Get(key));
    }

    template <typename T>
    void Set(const OmChunkCoord& chunkCoord,
             const int sliceDepth,
             const ViewType viewType, boost::shared_ptr<T> slice)
    {
        const OmVolSliceKey key(chunkCoord, sliceDepth, viewType);
        cache_->Set(key, om::ptrs::Wrap(slice));
    }

private:
    static OmVolSliceCacheBase* cacheFactory(OmMipVolume* vol)
    {
        switch(vol->getVolDataType().index()){
        case OmVolDataType::INT8:
            return new OmVolSliceCacheImpl<int8_t>(vol);
        case OmVolDataType::UINT8:
            return new OmVolSliceCacheImpl<uint8_t>(vol);
        case OmVolDataType::INT32:
            return new OmVolSliceCacheImpl<int32_t>(vol);
        case OmVolDataType::UINT32:
            return new OmVolSliceCacheImpl<uint32_t>(vol);
        case OmVolDataType::FLOAT:
            return new OmVolSliceCacheImpl<float>(vol);
        case OmVolDataType::UNKNOWN:
        default:
            throw OmIoException("data type not handled");
        };
    }
};

#endif
