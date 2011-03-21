#ifndef OM_SEGMENT_CACHE_IMPL_LOW_LEVEL_H
#define OM_SEGMENT_CACHE_IMPL_LOW_LEVEL_H

#include "utility/omLockedPODs.hpp"
#include "common/omCommon.h"
#include "segment/lowLevel/omPagingPtrStore.h"
#include "segment/lowLevel/omSegmentGraph.h"
#include "segment/omSegment.h"

#include <QHash>

class OmEnabledSegments;
class OmSegmentSelection;
class OmSegments;
class OmSegmentation;
class SegmentationDataWrapper;

class OmSegmentsImplLowLevel {
public:
    OmSegmentsImplLowLevel(OmSegmentation *);
    virtual ~OmSegmentsImplLowLevel();

    inline void RefreshGUIlists(){
        segmentGraph_.RefreshGUIlists();
    }

    void growGraphIfNeeded(OmSegment* newSeg);

    inline OmSegment* GetSegment(const OmSegID value){
        return segmentPages_->GetSegment(value);
    }

    inline OmSegID GetNumSegments() const {
        return mNumSegs;
    }
    void SetNumSegments(const uint32_t num){
        mNumSegs = num;
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

        return GetSegment(segmentGraph_.Root(segment->value()));
    }

    inline OmSegID FindRootID(OmSegment* segment)
    {
        if(!segment){
            return 0;
        }

        if(!segment->getParent()) {
            return segment->value();
        }

        return segmentGraph_.Root(segment->value());
    }

    inline OmSegment* FindRoot(const OmSegID segID)
    {
        if(!segID){
            return 0;
        }

        return GetSegment(segmentGraph_.Root(segID));
    }

    inline OmSegID FindRootID(const OmSegID segID)
    {
        if(!segID){
            return 0;
        }

        return segmentGraph_.Root(segID);
    }

    QString getSegmentName(OmSegID segID);
    void setSegmentName(OmSegID segID, QString name);

    QString getSegmentNote(OmSegID segID);
    void setSegmentNote(OmSegID segID, QString note);

    void turnBatchModeOn(const bool batchMode);

    uint32_t getPageSize();

    inline uint32_t getMaxValue() const {
        return maxValue_.get();
    }

    inline uint64_t MSTfreshness() const {
        return segmentGraph_.MSTfreshness();
    }

    inline OmSegmentSelection& SegmentSelection(){
        return *segmentSelection_;
    }

    inline OmEnabledSegments& EnabledSegments(){
        return *enabledSegments_;
    }

    std::vector<OmSegmentPage>& Pages();
    const std::set<PageNum> ValidPageNums() const;

    SegmentationDataWrapper GetSDW() const;

    OmPagingPtrStore* PagingPtrStore(){
        return segmentPages_.get();
    }

protected:
    OmSegmentation *const segmentation_;
    const boost::scoped_ptr<OmPagingPtrStore> segmentPages_;
    const boost::scoped_ptr<OmSegmentSelection> segmentSelection_;
    const boost::scoped_ptr<OmEnabledSegments> enabledSegments_;

    LockedUint32 maxValue_;
    uint32_t mNumSegs;

    QHash< OmID, QString > segmentCustomNames;
    QHash< OmID, QString > segmentNotes;

    inline OmSegID getNextValue(){
        return maxValue_.inc();
    }
    void touchFreshness();

    OmSegmentGraph segmentGraph_;

private:
    friend class OmSegmentSelection;
    friend class OmEnabledSegments;
    friend class SegmentTests;
};

#endif
