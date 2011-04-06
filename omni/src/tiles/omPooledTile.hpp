#ifndef OM_POOLED_TILE_HPP
#define OM_POOLED_TILE_HPP

#include "project/omProject.h"
#include "project/omProjectGlobals.h"
#include "tiles/omTilePool.hpp"

class OmPooledTileWrapper {
public:
    virtual ~OmPooledTileWrapper()
    {}

    virtual void* GetDataVoid() = 0;
};

template <typename T>
class OmPooledTile : public OmPooledTileWrapper {
private:
    T* tile_;

public:
    OmPooledTile()
        : tile_(OmProject::Globals().TilePool<T>().GetTile())
    {}

    virtual ~OmPooledTile(){
        OmProject::Globals().TilePool<T>().FreeTile(tile_);
    }

    T* GetData(){
        return tile_;
    }

    void* GetDataVoid(){
        return tile_;
    }
};

#endif
