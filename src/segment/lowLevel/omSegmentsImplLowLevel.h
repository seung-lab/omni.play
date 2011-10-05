#pragma once

#include "common/common.h"
#include "segment/lowLevel/store/omSegmentStore.hpp"
#include "segment/lowLevel/omSegmentGraph.h"
#include "segment/omSegment.h"
#include "utility/omLockedPODs.hpp"

#include <QHash>

class OmEnabledSegments;
class OmSegmentSelection;
class segments;
class segmentation;
class SegmentationDataWrapper;

class segmentsImplLowLevel {
public:
    segmentsImplLowLevel(segmentation*, segmentsStore*);
    virtual ~segmentsImplLowLevel();

    inline void RefreshGUIlists(){
        segmentGraph_.RefreshGUIlists();
    }

    void growGraphIfNeeded(OmSegment* newSeg);

    inline segId GetNumSegments() const {
        return mNumSegs;
    }

    void SetNumSegments(const uint32_t num){
        mNumSegs = num;
    }

    inline OmSegment* FindRoot(OmSegment* segment)
    {
        if(!segment){
            return 0;
        }

        if(!segment->getParent()) {
            return segment;
        }

        return store_->GetSegment(segmentGraph_.Root(segment->value()));
    }

    inline segId FindRootID(OmSegment* segment)
    {
        if(!segment){
            return 0;
        }

        if(!segment->getParent()) {
            return segment->value();
        }

        return segmentGraph_.Root(segment->value());
    }

    inline OmSegment* FindRoot(const segId segID)
    {
        if(!segID){
            return 0;
        }

        return store_->GetSegment(segmentGraph_.Root(segID));
    }

    inline segId FindRootID(const segId segID)
    {
        if(!segID){
            return 0;
        }

        return segmentGraph_.Root(segID);
    }

    QString getSegmentName(segId segID);
    void setSegmentName(segId segID, QString name);

    QString getSegmentNote(segId segID);
    void setSegmentNote(segId segID, QString note);

    void turnBatchModeOn(const bool batchMode);

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

    SegmentationDataWrapper GetSDW() const;

    segmentsStore* SegmentStore(){
        return store_;
    }

protected:
    segmentation *const segmentation_;
    segmentsStore *const store_;
    const boost::scoped_ptr<OmSegmentSelection> segmentSelection_;
    const boost::scoped_ptr<OmEnabledSegments> enabledSegments_;

    OmLockedUint32 maxValue_;
    uint32_t mNumSegs;

    QHash< OmID, QString > segmentCustomNames;
    QHash< OmID, QString > segmentNotes;

    inline segId getNextValue(){
        return maxValue_.inc();
    }

    void touchFreshness();

    OmSegmentGraph segmentGraph_;

private:
    friend class OmSegmentSelection;
    friend class OmEnabledSegments;
    friend class SegmentTests;
};

