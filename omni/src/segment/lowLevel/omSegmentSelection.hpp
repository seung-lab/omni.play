#ifndef OM_SEGMENT_SELECTION_HPP
#define OM_SEGMENT_SELECTION_HPP

#include "segment/lowLevel/omSegmentCacheImplLowLevel.h"
#include "zi/omUtility.h"

class OmSegmentCacheImpl;

class OmSegmentSelection {
private:
    OmSegmentCacheImplLowLevel *const cache_;

    OmSegIDsSet selected_;

    friend QDataStream& operator<<(QDataStream&, const OmSegmentCacheImpl&);
    friend QDataStream& operator>>(QDataStream&, OmSegmentCacheImpl&);

public:
    OmSegmentSelection(OmSegmentCacheImplLowLevel* cache)
        : cache_(cache)
    {}

    inline const OmSegIDsSet& GetSelectedSegmentIds() const {
        return selected_;
    }

    inline uint32_t numberOfSelectedSegments() const {
        return selected_.size();
    }

    inline void Clear(){
        selected_.clear();
    }

    void rerootSegmentList()
    {
        OmSegIDsSet old = selected_;
        selected_.clear();

        FOR_EACH(iter, old){
            selected_.insert(cache_->findRootID(*iter));
        }
    }

    inline bool AreSegmentsSelected() const {
        return selected_.size();
    }

    inline bool isSegmentSelected(OmSegment* seg) const {
        return isSegmentSelected(seg->value());
    }

    inline bool isSegmentSelected(const OmSegID segID) const
    {
        const OmSegID rootID = cache_->findRootID(segID);
        return selected_.count(rootID);
    }

    inline void setSegmentSelected(OmSegID segID,
                                   const bool isSelected,
                                   const bool addToRecentList)
    {
        setSegmentSelectedBatch(segID, isSelected, addToRecentList);
        cache_->touchFreshness();
    }

    void UpdateSegmentSelection(const OmSegIDsSet & ids,
                                const bool addToRecentList)
    {
        selected_.clear();

        FOR_EACH(iter, ids){
            setSegmentSelectedBatch(*iter, true, addToRecentList);
        }

        cache_->touchFreshness();
    }

    void AddToSegmentSelection(const OmSegIDsSet& ids)
    {
        FOR_EACH(iter, ids){
            setSegmentSelectedBatch(*iter, true, false);
        }

        cache_->touchFreshness();
    }

    void RemoveFromSegmentSelection(const OmSegIDsSet& ids)
    {
        FOR_EACH(iter, ids){
            setSegmentSelectedBatch(*iter, false, false);
        }

        cache_->touchFreshness();
    }

    inline void setSegmentSelectedBatch(const OmSegID segID,
                                        const bool isSelected,
                                        const bool addToRecentList)
    {
        const OmSegID rootID = cache_->findRootID(segID);

        if (isSelected) {
            doSelectedSetInsert(rootID, addToRecentList);
        } else {
            doSelectedSetRemove(rootID);
        }
    }

    inline void doSelectedSetInsert(const OmSegID segID,
                                    const bool addToRecentList)
    {
        selected_.insert(segID);
        if(addToRecentList) {
            cache_->addToRecentMap(segID);
        }
    }

    inline void doSelectedSetRemove(const OmSegID segID)
    {
        selected_.erase(segID);
        cache_->addToRecentMap(segID);
    }
};

#endif
