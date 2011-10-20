#pragma once

#include "common/common.h"
#include "segment/lowLevel/segmentGraph.h"
#include "segment/segment.h"
#include "utility/omLockedPODs.hpp"

class enabledSegments;
class segmentSelection;
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

    void growGraphIfNeeded(segment* newSeg);

    inline common::segId GetNumSegments() const {
        return mNumSegs;
    }

    void SetNumSegments(const uint32_t num){
        mNumSegs = num;
    }

    inline segment* FindRoot(segment* segment)
    {
        if(!segment){
            return 0;
        }

        if(!segment->getParent()) {
            return segment;
        }

        return store_->GetSegment(segmentGraph_.Root(segment->value()));
    }

    inline common::segId FindRootID(segment* segment)
    {
        if(!segment){
            return 0;
        }

        if(!segment->getParent()) {
            return segment->value();
        }

        return segmentGraph_.Root(segment->value());
    }

    inline segment* FindRoot(const common::segId segID)
    {
        if(!segID){
            return 0;
        }

        return store_->GetSegment(segmentGraph_.Root(segID));
    }

    inline common::segId FindRootID(const common::segId segID)
    {
        if(!segID){
            return 0;
        }

        return segmentGraph_.Root(segID);
    }

    std::string getSegmentName(common::segId segID);
    void setSegmentName(common::segId segID, std::string name);

    std::string getSegmentNote(common::segId segID);
    void setSegmentNote(common::segId segID, std::string note);

    void turnBatchModeOn(const bool batchMode);

    inline uint32_t getMaxValue() const {
        return maxValue_.get();
    }

    inline uint64_t MSTfreshness() const {
        return segmentGraph_.MSTfreshness();
    }

    inline segmentSelection& SegmentSelection(){
        return *segmentSelection_;
    }

    inline enabledSegments& EnabledSegments(){
        return *enabledSegments_;
    }

    SegmentationDataWrapper GetSDW() const;

    segmentsStore* SegmentStore(){
        return store_;
    }

protected:
    segmentation *const segmentation_;
    segmentsStore *const store_;
    const boost::scoped_ptr<segmentSelection> segmentSelection_;
    const boost::scoped_ptr<enabledSegments> enabledSegments_;

    OmLockedUint32 maxValue_;
    uint32_t mNumSegs;

    QHash< common::id, std::string > segmentCustomNames;
    QHash< common::id, std::string > segmentNotes;

    inline common::segId getNextValue(){
        return maxValue_.inc();
    }

    void touchFreshness();

    segmentGraph segmentGraph_;

private:
    friend class segmentSelection;
    friend class enabledSegments;
    friend class SegmentTests;
};

