#ifndef OM_ENABLED_SEGMENTS_HPP
#define OM_ENABLED_SEGMENTS_HPP

#include "segment/lowLevel/omSegmentCacheImplLowLevel.h"
#include "zi/omUtility.h"

class OmSegmentCacheImpl;

class OmEnabledSegments {
private:
    OmSegmentCacheImplLowLevel *const cache_;

    OmSegIDsSet enabled_;

    friend QDataStream& operator<<(QDataStream&, const OmSegmentCacheImpl&);
    friend QDataStream& operator>>(QDataStream&, OmSegmentCacheImpl&);

public:
    OmEnabledSegments(OmSegmentCacheImplLowLevel* cache)
        : cache_(cache)
    {}

    inline void Reroot()
    {
        OmSegIDsSet old = enabled_;
        enabled_.clear();

        FOR_EACH(iter, old){
            enabled_.insert(cache_->FindRootID(*iter));
        }
    }

    inline OmSegIDsSet& GetEnabledSegmentIdsRef(){
        return enabled_;
    }

    inline uint32_t Size() const {
        return enabled_.size();
    }

    inline bool IsEnabled(const OmSegID segID) const
    {
        if(enabled_.empty()){
            return false;
        }
        return enabled_.count(cache_->FindRootID(segID));
    }

    inline bool AnyEnabled() const {
        return enabled_.size();
    }

    inline void SetEnabled(const OmSegID segID, const bool isEnabled)
    {
        const OmSegID rootID = cache_->FindRootID(segID);

        cache_->touchFreshness();

        if (isEnabled) {
            enabled_.insert(rootID);
        } else {
            enabled_.erase(rootID);
        }
    }
};

#endif
