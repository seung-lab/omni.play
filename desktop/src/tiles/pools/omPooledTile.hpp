#pragma once

#include "tiles/pools/omTilePool.hpp"
#include "tiles/pools/omTilePools.hpp"

class OmPooledTileWrapper {
public:
    virtual ~OmPooledTileWrapper()
    {}

    virtual void* GetDataVoid() = 0;
};

template <typename T>
class OmPooledTile : public OmPooledTileWrapper {
private:
    T *const tile_;

public:
    OmPooledTile()
        : tile_(OmTilePools::GetPool<T>().GetTile())
    {}

    virtual ~OmPooledTile(){
        OmTilePools::GetPool<T>().FreeTile(tile_);
    }

    T* GetData(){
        return tile_;
    }

    void* GetDataVoid(){
        return tile_;
    }
};

