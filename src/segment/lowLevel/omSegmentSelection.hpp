#pragma once

#include "segment/lists/omSegmentLists.h"
#include "segment/lowLevel/segmentsImplLowLevel.h"
#include "volume/segmentation.h"
#include "zi/omUtility.h"
#include "datalayer/archive/segmentation.h"

class segmentsImpl;

class OmSegmentSelection {
private:
    segmentsImplLowLevel *const cache_;

    segIdsSet selected_;

    friend YAML::Emitter& YAML::operator<<(YAML::Emitter&, const segmentsImpl&);
    friend void YAML::operator>>(const YAML::Node&, segmentsImpl&);
    friend QDataStream& operator<<(QDataStream&, const segmentsImpl&);
    friend QDataStream& operator>>(QDataStream&, segmentsImpl&);
    
    inline void addToRecentMap(const segId segID)
    {
        OmSegment* seg = cache_->SegmentStore()->GetSegment(segID);
        cache_->segmentation_->SegmentLists()->TouchRecent(seg);
    }

public:
    OmSegmentSelection(segmentsImplLowLevel* cache)
        : cache_(cache)
    {}

    inline const segIdsSet GetSelectedSegmentIDs() const {
        return selected_;
    }

    inline uint32_t NumberOfSelectedSegments() const {
        return selected_.size();
    }

    inline void Clear(){
        selected_.clear();
    }

    void rerootSegmentList()
    {
        segIdsSet old = selected_;
        selected_.clear();

        FOR_EACH(iter, old){
            selected_.insert(cache_->FindRootID(*iter));
        }
    }

    inline bool AreSegmentsSelected() const {
        return !selected_.empty();
    }

    inline bool isSegmentSelected(const segId segID) const
    {
        if(selected_.empty()){
            return false;
        }
        return selected_.count(cache_->FindRootID(segID));
    }

    inline void setSegmentSelected(segId segID,
                                   const bool isSelected,
                                   const bool addToRecentList)
    {
        setSegmentSelectedBatch(segID, isSelected, addToRecentList);
        cache_->touchFreshness();
    }

    void UpdateSegmentSelection(const segIdsSet & ids,
                                const bool addToRecentList)
    {
        selected_.clear();

        FOR_EACH(iter, ids){
            setSegmentSelectedBatch(*iter, true, addToRecentList);
        }

        cache_->touchFreshness();
    }

    void AddToSegmentSelection(const segIdsSet& ids)
    {
        FOR_EACH(iter, ids){
            setSegmentSelectedBatch(*iter, true, true);
        }

        cache_->touchFreshness();
    }

    void RemoveFromSegmentSelection(const segIdsSet& ids)
    {
        FOR_EACH(iter, ids){
            setSegmentSelectedBatch(*iter, false, false);
        }

        cache_->touchFreshness();
    }

    inline void setSegmentSelectedBatch(const segId segID,
                                        const bool isSelected,
                                        const bool addToRecentList)
    {
        const segId rootID = cache_->FindRootID(segID);

        if (isSelected) {
            doSelectedSetInsert(rootID, addToRecentList);
        } else {
            doSelectedSetRemove(rootID);
        }
    }

    inline void doSelectedSetInsert(const segId segID,
                                    const bool addToRecentList)
    {
        selected_.insert(segID);
        if(addToRecentList) {
            addToRecentMap(segID);
        }
    }

    inline void doSelectedSetRemove(const segId segID)
    {
        selected_.erase(segID);
        addToRecentMap(segID);
    }
};

