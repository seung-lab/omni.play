#pragma once

#include "segment/lowLevel/omSegmentsImplLowLevel.h"
#include "zi/omUtility.h"
#include "datalayer/archive/segmentation.h"

class OmSegmentsImpl;

class OmEnabledSegments {
private:
    OmSegmentsImplLowLevel *const cache_;

    OmSegIDsSet enabled_;

    friend YAML::Emitter& om::data::archive::operator<<(YAML::Emitter&, const OmSegmentsImpl&);
    friend void om::data::archive::operator>>(const YAML::Node&, OmSegmentsImpl&);
    friend QDataStream& operator<<(QDataStream&, const OmSegmentsImpl&);
    friend QDataStream& operator>>(QDataStream&, OmSegmentsImpl&);
    
public:
    OmEnabledSegments(OmSegmentsImplLowLevel* cache)
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

    inline OmSegIDsSet GetEnabledSegmentIDs(){
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

