#ifndef OM_TILE_POOL_HPP
#define OM_TILE_POOL_HPP

#include "utility/omLockedPODs.hpp"
#include "utility/omPrimeNumbers.hpp"
#include "utility/omSystemInformation.h"
#include "zi/omMutex.h"

#include <boost/ptr_container/ptr_list.hpp>

template <typename T>
class MallocArrayWrapper {
private:
    T *const v_;

public:
    MallocArrayWrapper(T* v)
        : v_(v)
    {}

    ~MallocArrayWrapper(){
        free(v_);
    }
};

template <typename T>
class OmTilePool {
private:
    const uint32_t numElementsPerTile_;
    const uint32_t numBytesPerTile_;
    const uint32_t numBytesPerPage_;
    const uint32_t numTilesPerPage_;
    const uint32_t numElementsPerPage_;

    const uint64_t numBuckets_;

    zi::spinlock pagesLock_;
    boost::ptr_list<MallocArrayWrapper<T> > pages_;

    struct FreeTileList {
        std::deque<T*> freeTiles;
        zi::spinlock lock;
    };

    std::vector<FreeTileList*> freeTilesLists_; // size is fixed

    LockedUint32 curIndex_;

    // needs to return prime number
    uint64_t computeNumBuckets()
    {
        const int numCores = OmSystemInformation::get_num_cores();
        return om::constants::getNextBiggestPrime(2 * numCores);
    }

public:
    OmTilePool(const int megsPerPage,
               const int numElementsPerTile)
        : numElementsPerTile_(numElementsPerTile)
        , numBytesPerTile_(sizeof(T) * numElementsPerTile)
        , numBytesPerPage_(megsPerPage * BYTES_PER_MB)
        , numTilesPerPage_(numBytesPerPage_ / numBytesPerTile_)
        , numElementsPerPage_(numTilesPerPage_ * numElementsPerTile)
        , numBuckets_(computeNumBuckets())
    {
        if(0 != numBytesPerPage_ % numBytesPerTile_){
            throw OmArgException("invalid size");
        }

        freeTilesLists_.resize(numBuckets_);
        for(uint32_t i = 0; i < numBuckets_; ++i){
            freeTilesLists_[i] = new FreeTileList();
        }

        curIndex_.set(0);
    }

    ~OmTilePool()
    {
        for(uint32_t i = 0; i < numBuckets_; ++i){
            delete freeTilesLists_[i];
        }
    }

    T* GetTile()
    {
        T* tile = doGet();

        if(!tile){
            tile = allocatePage();
        }

        return tile;
    }

    void FreeTile(T* tile)
    {
        const int index = reinterpret_cast<uint64_t>(tile) % numBuckets_;

        FreeTileList* ftl = freeTilesLists_[ index ];

        {
            zi::guard g(ftl->lock);
            ftl->freeTiles.push_back(tile);
        }
    }

private:
    T* allocatePage()
    {
        T* newPage = static_cast<T*>(malloc(numBytesPerPage_));
        if(!newPage){
            throw std::bad_alloc();
        }

        {
            zi::guard g(pagesLock_);
            pages_.push_back(new MallocArrayWrapper<T>(newPage));
        }

        for(uint32_t i = numElementsPerTile_; // skip first tile
            i < numElementsPerPage_;
            i += numElementsPerTile_)
        {
            T* tile = &newPage[i];

            const int index = reinterpret_cast<uint64_t>(tile) % numBuckets_;

            FreeTileList* ftl = freeTilesLists_[ index ];

            {
                zi::guard g(ftl->lock);
                ftl->freeTiles.push_back(tile);
            }
        }

        return newPage;
    }

    T* doGet()
    {
        FreeTileList* ftl = freeTilesLists_[ curIndex_.inc() % numBuckets_ ];

        T* tile = NULL;
        {
            zi::guard g(ftl->lock);
            if(!ftl->freeTiles.empty())
            {
                tile = ftl->freeTiles.back();
                ftl->freeTiles.pop_back();
            }
        }

        return tile;
    }
};

#endif
