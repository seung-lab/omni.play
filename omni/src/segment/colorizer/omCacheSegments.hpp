#ifndef OM_LOCAL_SEGMENT_CACHE_HPP
#define OM_LOCAL_SEGMENT_CACHE_HPP

#include "common/omCommon.h"
#include "segment/omSegments.h"

class OmCacheSegments : public zi::rwmutex {
private:
    OmSegments *const segments_;

    struct OmSegIDFreshness {
        uint64_t segMSTfreshness;
        OmSegID rootSegID;
    };

    std::vector<OmSegIDFreshness> rootIDcache_;
    std::vector<OmSegment*> cacheSegments_;

    uint64_t mstFreshness_;
    uint32_t size_;

    struct om_local_segment_cache_root_id;
    typedef zi::spinlock::pool<om_local_segment_cache_root_id>::guard root_spinlock_guard_t;

    struct om_local_segment_cache_seg;
    typedef zi::spinlock::pool<om_local_segment_cache_seg>::guard seg_spinlock_guard_t;

public:
    OmCacheSegments(OmSegments* segments)
        : segments_(segments)
        , mstFreshness_(0)
        , size_(0)
    {}

    void CheckSizeAndMSTfreshness(const uint32_t newSize)
    {
        zi::rwmutex::write_guard g(*this);

        if(newSize != size_){
            size_ = newSize;
            rootIDcache_.resize(newSize);
            cacheSegments_.resize(newSize);
        }

        mstFreshness_ = segments_->MSTfreshness();
    }

    inline OmSegment* GetSegment(const OmSegID val){
        return getSegment(val);
    }

    inline OmSegment* GetRootSegment(OmSegment* seg)
    {
        if(!seg->getParent()){
            return seg;
        }
        const OmSegID rootID = getRootID(seg->value());
        return getSegment(rootID);
    }

private:
    inline OmSegment* getSegment(const OmSegID val)
    {
        OmSegment* seg;
        {
            seg_spinlock_guard_t g(val);
            seg = cacheSegments_[val];
        }

        if(!seg){
            seg = segments_->GetSegment(val);
            {
                seg_spinlock_guard_t g(val);
                cacheSegments_[val] = seg;
            }
        }

        return seg;
    }

    inline OmSegID getRootID(const OmSegID val)
    {
        OmSegID rootID;
        uint64_t segMSTfreshness;
        {
            root_spinlock_guard_t g(val);
            rootID = rootIDcache_[val].rootSegID;
            segMSTfreshness = rootIDcache_[val].segMSTfreshness;
        }

        if(!rootID || segMSTfreshness != mstFreshness_)
        {
            rootID = segments_->findRootID(val);
            {
                root_spinlock_guard_t g(val);
                rootIDcache_[val].rootSegID = rootID;
                rootIDcache_[val].segMSTfreshness = mstFreshness_;
            }
        }

        return rootID;
    }
};

#endif
