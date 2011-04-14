#ifndef OM_TILE_POOLS_HPP
#define OM_TILE_POOLS_HPP

#include "tiles/omTilePool.hpp"

class OmTilePools {
private:
    boost::scoped_ptr<OmTilePool<int8_t> > tilePoolInt8_;
    boost::scoped_ptr<OmTilePool<uint8_t> > tilePoolUint8_;
    boost::scoped_ptr<OmTilePool<int32_t> > tilePoolInt32_;
    boost::scoped_ptr<OmTilePool<uint32_t> > tilePoolUint32_;
    boost::scoped_ptr<OmTilePool<float> > tilePoolFloat_;
    boost::scoped_ptr<OmTilePool<OmColorARGB> > tilePoolARGB_;

public:
    OmTilePools()
        : tilePoolInt8_(new OmTilePool<int8_t>(20, 128*128))
        , tilePoolUint8_(new OmTilePool<uint8_t>(20, 128*128))
        , tilePoolInt32_(new OmTilePool<int32_t>(20, 128*128))
        , tilePoolUint32_(new OmTilePool<uint32_t>(20, 128*128))
        , tilePoolFloat_(new OmTilePool<float>(20, 128*128))
        , tilePoolARGB_(new OmTilePool<OmColorARGB>(20, 128*128))
    {}

    template <typename T> OmTilePool<T>& GetPool(){
        return getTilePool(static_cast<T*>(0));
    }

private:
    inline OmTilePool<int8_t>& getTilePool(int8_t*){
        return *tilePoolInt8_;
    }

    inline OmTilePool<uint8_t>& getTilePool(uint8_t*){
        return *tilePoolUint8_;
    }

    inline OmTilePool<int32_t>& getTilePool(int32_t*){
        return *tilePoolInt32_;
    }

    inline OmTilePool<uint32_t>& getTilePool(uint32_t*){
        return *tilePoolUint32_;
    }

    inline OmTilePool<float>& getTilePool(float*){
        return *tilePoolFloat_;
    }

    inline OmTilePool<OmColorARGB>& getTilePool(OmColorARGB*){
        return *tilePoolARGB_;
    }
};

#endif
