#pragma once

#include "segment/lowLevel/segmentsImplLowLevel.h"
#include "zi/omUtility.h"
#include "datalayer/archive/segmentation.h"

class segmentsImpl;

class OmEnabledSegments {
private:
    segmentsImplLowLevel *const cache_;

    segIdsSet enabled_;

    friend YAML::Emitter& YAML::operator<<(YAML::Emitter&, const segmentsImpl&);
    friend void YAML::operator>>(const YAML::Node&, segmentsImpl&);
    friend QDataStream& operator<<(QDataStream&, const segmentsImpl&);
    friend QDataStream& operator>>(QDataStream&, segmentsImpl&);
    
public:
    OmEnabledSegments(segmentsImplLowLevel* cache)
        : cache_(cache)
    {}

    inline void Reroot()
    {
        segIdsSet old = enabled_;
        enabled_.clear();

        FOR_EACH(iter, old){
            enabled_.insert(cache_->FindRootID(*iter));
        }
    }

    inline segIdsSet GetEnabledSegmentIDs(){
        return enabled_;
    }

    inline uint32_t Size() const {
        return enabled_.size();
    }

    inline bool IsEnabled(const segId segID) const
    {
        if(enabled_.empty()){
            return false;
        }
        return enabled_.count(cache_->FindRootID(segID));
    }

    inline bool AnyEnabled() const {
        return enabled_.size();
    }

    inline void SetEnabled(const segId segID, const bool isEnabled)
    {
        const segId rootID = cache_->FindRootID(segID);

        cache_->touchFreshness();

        if (isEnabled) {
            enabled_.insert(rootID);
        } else {
            enabled_.erase(rootID);
        }
    }
};

