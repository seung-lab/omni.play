#ifndef OM_CHUNK_CACHE_HPP
#define OM_CHUNK_CACHE_HPP

#include "chunks/omChunkCoord.h"
#include "zi/omMutex.h"

#include <boost/multi_array.hpp>

/**
 * cache is a mutex-lock map of chunk coords to OmChunk objects
 **/

template <typename VOL, typename CHUNK>
class OmChunkCache{
private:
    VOL *const vol_;;

    int rootMipLevel_;

    zi::spinlock lock_;

    typedef boost::multi_array<boost::shared_ptr<CHUNK>, 3> array_t;
    typedef typename array_t::index index;

    std::vector<array_t*> mips_;
    std::vector<Vector3i> mipDims_;

    void setup()
    {
        zi::guard g(lock_);

        for(size_t i = 0; i < mips_.size(); ++i){
            delete mips_[i];
        }

        rootMipLevel_ = vol_->Coords().GetRootMipLevel();

        mips_ = std::vector<array_t*>();
        mips_.resize(rootMipLevel_ + 1);

        mipDims_ = std::vector<Vector3i>();
        mipDims_.resize(rootMipLevel_ + 1);

        for(int level = 0; level <= rootMipLevel_; ++level)
        {
            const Vector3i dims = vol_->Coords().MipLevelDimensionsInMipChunks(level);
            mipDims_[level] = dims;

            mips_[level] = new array_t(boost::extents[dims.x][dims.y][dims.z]);
        }
    }

public:
    OmChunkCache(VOL* vol)
        : vol_(vol)
    {
        setup();
    }

    ~OmChunkCache()
    {
        for(size_t i = 0; i < mips_.size(); ++i){
            delete mips_[i];
        }
    }

    void UpdateFromVolResize(){
        setup();
    }

    void Get(boost::shared_ptr<CHUNK>& ptr, const OmChunkCoord& coord)
    {
        const size_t level = coord.Level;
        const index x = coord.Coordinate.x;
        const index y = coord.Coordinate.y;
        const index z = coord.Coordinate.z;

        {
            zi::guard g(lock_);

            assert(level < mipDims_.size());

            const Vector3i& dims = mipDims_[level];
            assert(x < dims.x);
            assert(y < dims.y);
            assert(z < dims.z);

            array_t& array = *mips_[level];

            if(!array[x][y][z]){
                array[x][y][z] = boost::make_shared<CHUNK>(vol_, coord);
            }

            ptr = array[x][y][z];
        }
    }
};

#endif
