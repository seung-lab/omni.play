#pragma once

#include "common/common.h"
#include "common/omContainer.hpp"
#include "src/chunks/chunkItemContainerMatrix.hpp"
#include "zi/omMutex.h"

template <typename VOL, typename T>
class chunkItemContainer {
private:
    VOL *const vol_;;

    zi::spinlock lock_;

    typedef chunkItemContainerMatrix<VOL,T> matrix_t;
    std::vector<matrix_t*> mips_;

    void setup()
    {
        const int rootMipLevel = vol_->Coords().GetRootMipLevel();

        std::vector<matrix_t*> newMips;
        newMips.resize(rootMipLevel + 1);

        for(int mipLevel = 0; mipLevel <= rootMipLevel; ++mipLevel)
        {
            const Vector3i dims = vol_->Coords().MipLevelDimensionsInMipChunks(mipLevel);
            newMips[mipLevel] = new matrix_t(vol_, dims.x, dims.y, dims.z);
        }

        {
            zi::guard g(lock_);
            mips_.swap(newMips);
        }

        om::container::clearPtrVec(newMips);
    }

public:
    chunkItemContainer(VOL* vol)
        : vol_(vol)
    {
        setup();
    }

    ~chunkItemContainer(){
        om::container::clearPtrVec(mips_);
    }

    void UpdateFromVolResize(){
        setup();
    }

    void Clear(){
        setup();
    }

    T* Get(const coords::chunkCoord& coord)
    {
        zi::guard g(lock_);

        const std::size_t mipLevel = coord.Level;

        if(mipLevel >= mips_.size()){
            throw OmArgException("invalid mip level");
        }

        return mips_[mipLevel]->Get(coord);
    }
};

