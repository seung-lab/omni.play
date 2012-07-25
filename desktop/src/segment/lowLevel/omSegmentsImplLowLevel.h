#pragma once

#include "common/omCommon.h"
#include "segment/lowLevel/store/omSegmentStore.hpp"
#include "segment/lowLevel/omSegmentGraph.h"
#include "segment/omSegment.h"
#include "utility/omLockedPODs.hpp"

#include <QHash>

class OmEnabledSegments;
class OmSegmentSelection;
class OmSegments;
class OmSegmentation;
class SegmentationDataWrapper;

class OmSegmentsImplLowLevel {
public:
    OmSegmentsImplLowLevel(OmSegmentation*, OmSegmentsStore*);
    virtual ~OmSegmentsImplLowLevel();

    inline void RefreshGUIlists(){
        segmentGraph_.RefreshGUIlists();
    }

    void growGraphIfNeeded(OmSegment* newSeg);

    inline OmSegID GetNumSegments() const {
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

        return store_->GetSegment(segmentGraph_.Root(segID));
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

    inline uint32_t getMaxValue() const {
        return maxValue_.get();
    }

    inline OmSegmentGraph* GetGraph()
    {
        return &segmentGraph_;
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

    OmSegmentsStore* SegmentStore(){
        return store_;
    }

    // Your method here

    inline void AddNeighboursToSelection(OmMST* mst, OmSegmentSelector* sel, OmSegID SegmentID)
    {
        segmentGraph_.AddNeighboursToSelection(mst,sel,SegmentID);
    }

    inline void AddSegments_BreadthFirstSearch(OmMST* mst, OmSegmentSelector* sel, OmSegID SegmentID)
    {
        segmentGraph_.AddSegments_BreadthFirstSearch(mst,sel,SegmentID);
    }

    inline void Grow_LocalSizeThreshold(OmMST* mst, OmSegmentSelector* sel, OmSegID SegmentID)
    {
        segmentGraph_.Grow_LocalSizeThreshold(mst,sel,SegmentID);
    }

    inline void AddSegments_DepthFirstSearch(OmMST* mst, OmSegmentSelector* sel, OmSegID SegmentID)
    {
        segmentGraph_.AddSegments_DepthFirstSearch(mst,sel,SegmentID);
    }

    inline void AddSegments_BFS_DynamicThreshold(OmMST* mst, OmSegmentSelector* sel, OmSegID SegmentID)
    {
        segmentGraph_.AddSegments_BFS_DynamicThreshold(mst,sel,SegmentID);
    }

    inline void Trim(OmMST* mst, OmSegmentSelector* sel, OmSegID SegmentID)
    {
        segmentGraph_.Trim(mst,sel,SegmentID);
    }

protected:
    OmSegmentation *const segmentation_;
    OmSegmentsStore *const store_;
    const boost::scoped_ptr<OmSegmentSelection> segmentSelection_;
    const boost::scoped_ptr<OmEnabledSegments> enabledSegments_;

    OmLockedUint32 maxValue_;
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

