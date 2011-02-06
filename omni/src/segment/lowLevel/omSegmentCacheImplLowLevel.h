#ifndef OM_SEGMENT_CACHE_IMPL_LOW_LEVEL_H
#define OM_SEGMENT_CACHE_IMPL_LOW_LEVEL_H

#include "common/omCommon.h"
#include "segment/lowLevel/omPagingPtrStore.h"
#include "segment/lowLevel/omSegmentGraph.h"
#include "segment/omSegment.h"

#include <QHash>

class OmEnabledSegments;
class OmSegmentSelection;
class OmSegmentCache;
class OmSegmentation;

class OmSegmentCacheImplLowLevel {
public:
    OmSegmentCacheImplLowLevel(OmSegmentation *);
    virtual ~OmSegmentCacheImplLowLevel();

    void growGraphIfNeeded(OmSegment * newSeg);

    inline OmSegment* GetSegment(const OmSegID value)
    {
        if(value > mMaxValue){
            throw OmIoException("bad segment value");
        }
        return mSegments->GetSegment(value);
    }

    inline OmSegID GetNumSegments() const {
        return mNumSegs;
    }

    OmSegID GetNumTopSegments();

    inline OmSegment* FindRoot(OmSegment* segment)
    {
        if(!segment){
            return 0;
        }

        if(!segment->getParent()) {
            return segment;
        }

        return GetSegment(mSegmentGraph.Root(segment->value()));
    }

    inline OmSegID FindRootID(OmSegment* segment)
    {
        if(!segment){
            return 0;
        }

        if(!segment->getParent()) {
            return segment->value();
        }

        return mSegmentGraph.Root(segment->value());
    }

    inline OmSegment* FindRoot(const OmSegID segID)
    {
        if(!segID){
            return 0;
        }

        return GetSegment(mSegmentGraph.Root(segID));
    }

    inline OmSegID FindRootID(const OmSegID segID)
    {
        if(!segID){
            return 0;
        }

        return mSegmentGraph.Root(segID);
    }

    QString getSegmentName(OmSegID segID);
    void setSegmentName(OmSegID segID, QString name);

    QString getSegmentNote(OmSegID segID);
    void setSegmentNote(OmSegID segID, QString note);

    void turnBatchModeOn(const bool batchMode);

    uint32_t getPageSize();

    inline uint32_t getMaxValue() const {
        return mMaxValue;
    }

    inline OmSegmentSelection& SegmentSelection(){
        return *segmentSelection_;
    }

    inline OmEnabledSegments& EnabledSegments(){
        return *enabledSegments_;
    }

protected:
    OmSegmentation *const segmentation_;
    const boost::scoped_ptr<OmPagingPtrStore> mSegments;
    const boost::scoped_ptr<OmSegmentSelection> segmentSelection_;
    const boost::scoped_ptr<OmEnabledSegments> enabledSegments_;

    OmSegID mMaxValue;
    uint32_t mNumSegs;

    QHash< OmID, QString > segmentCustomNames;
    QHash< OmID, QString > segmentNotes;

    inline OmSegID getNextValue(){
        return ++mMaxValue;
    }
    void touchFreshness();

    OmSegmentGraph mSegmentGraph;

    void addToRecentMap(const OmSegID segID);

private:
    friend class OmSegmentSelection;
    friend class OmEnabledSegments;
    friend class SegmentTests;
};

#endif
