#ifndef OM_CACHE_BASE_H
#define OM_CACHE_BASE_H

/**
 * Base class of all caches.
 * Brett Warne - bwarne@mit.edu - 7/15/09
 */

#include "common/omCommon.h"

class OmCacheBase {
protected:
    const std::string cacheName_;
    const om::CacheGroup cacheGroup_;

public:
    OmCacheBase(const std::string& cacheName, const om::CacheGroup group)
        : cacheName_(cacheName)
        , cacheGroup_(group)
    {}

    virtual ~OmCacheBase()
    {}

    virtual void UpdateSize(const int64_t)
    {}

    virtual int Clean(const bool) = 0;
    virtual void Clear() = 0;

    virtual int64_t GetCacheSize() const = 0;

    virtual void closeDownThreads()
    {}

    virtual const std::string& GetName() const {
        return cacheName_;
    }

    std::string getGroupName() const
    {
        if(om::MESH_CACHE == cacheGroup_){
            return "MESH_CACHE";
        }
        return "TILE_CACHE";
    }

    friend std::ostream& operator<<(std::ostream &out, const OmCacheBase& in)
    {
        out << in.GetName()
            << " ("
            << in.getGroupName()
            << ")";
        return out;
    }
};

#endif
