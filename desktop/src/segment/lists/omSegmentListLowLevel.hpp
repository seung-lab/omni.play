#pragma once

#include "events/omEvents.h"
#include "segment/io/omSegmentPage.hpp"
#include "segment/lists/omSegmentListForGUI.hpp"
#include "segment/lists/omSegmentListGlobal.hpp"
#include "segment/lists/omSegmentLists.h"
#include "segment/lowLevel/omSegmentsImplLowLevel.h"
#include "threads/omTaskManager.hpp"
#include "utility/omTimer.hpp"
#include "utility/segmentationDataWrapper.hpp"

class OmSegmentListLowLevel {
private:
    OmThreadPool threadPool_;

    OmSegmentLists *const segmentLists_;

    OmSegmentsImplLowLevel* cache_;

    std::vector<SegInfo> list_;
    bool recreateGUIlists_;

public:
    OmSegmentListLowLevel(OmSegmentLists* segmentLists)
        : segmentLists_(segmentLists)
        , cache_(NULL)
        , recreateGUIlists_(true)
    { }

    ~OmSegmentListLowLevel(){
        threadPool_.join();
    }
    
    void Init(OmSegmentsImplLowLevel* cache, const size_t size)
    {
        cache_ = cache;
        
        doResize(size);
        doBuildInitialSegmentList();
        
        // use threadPool as serial job queue;
        // more than 1 thread requires locking, and dealing w/ out-of-order
        //  manipulations of tree modifiations that are order-sensative
        threadPool_.start(1);
    }

    void Resize(const size_t size)
    {
        threadPool_.push_back(
            zi::run_fn(
                zi::bind(&OmSegmentListLowLevel::doResize,
                         this, size)));
    }

    inline void UpdateSizeListsFromJoin(OmSegment* root, OmSegment* child)
    {
        threadPool_.push_back(
            zi::run_fn(
                zi::bind(&OmSegmentListLowLevel::doUpdateSizeListsFromJoin,
                         this, root, child)));
    }

    inline void UpdateSizeListsFromSplit(OmSegment* root, OmSegment* child,
                                         const SizeAndNumPieces& childInfo)
    {
        threadPool_.push_back(
            zi::run_fn(
                zi::bind(&OmSegmentListLowLevel::doUpdateSizeListsFromSplit,
                         this, root, child, childInfo)));
    }

    void AddSegment(OmSegment* seg)
    {
        threadPool_.push_back(
            zi::run_fn(
                zi::bind(&OmSegmentListLowLevel::doAddSegment,
                         this, seg)));
    }

    inline void RefreshGUIlists(){
        runRefreshGUIlists(om::common::DONT_FORCE);
    }

    inline void ForceRefreshGUIlists(){
        runRefreshGUIlists(om::common::FORCE);
    }
    
    inline int64_t GetSizeWithChildren(const om::common::SegID segID) {
//         if(segID >= list_.size()){
//             std::cout << "segment " << segID << "not found\n";
//             return 0;
//         }
        return list_[segID].sizeIncludingChildren;
    }
    
    inline int64_t GetSizeWithChildren(OmSegment* seg){
        return GetSizeWithChildren(seg->value());
    }

private:
    inline void runRefreshGUIlists(const om::common::ShouldForce force)
    {
        threadPool_.push_back(
            zi::run_fn(
                zi::bind(&OmSegmentListLowLevel::doRecreateGUIlists,
                         this, force)));
    }

    // avoid locking by adding to serial job list
    void doResize(const size_t size){
        list_.resize(size);
    }

    void doAddSegment(OmSegment* seg)
    {
        const om::common::SegID segID = seg->value();

        if(segID >= list_.size()){
            list_.resize(segID + 10);
        }

        addSegment(seg);

        recreateGUIlists_ = true;
    }

    inline void addSegment(OmSegment* seg)
    {
        int64_t size = seg->size();
        if(!size){
            size = 1; // for newly-added segment
        }

        const SegInfo info = { seg, seg->value(), size, 0 };
        list_[seg->value()] = info;
    }

    void doBuildInitialSegmentList()
    {
        const std::vector<OmSegmentPage*> pages = cache_->SegmentStore()->Pages();
        const uint32_t pageSize = cache_->SegmentStore()->PageSize();

        uint32_t numSegs = 0;

        FOR_EACH(iter, pages)
        {
            OmSegmentPage* pagePtr = *iter;
            if(!pagePtr){
                continue;
            }

            OmSegmentPage& page = *pagePtr;

            for(uint32_t i = 0; i < pageSize; ++i)
            {
                OmSegment* seg = &(page[i]);
                if(seg->value()) //seg will never be NULL
                {
                    ++numSegs;
                    addSegment(seg);
                }
            }
        }

        // saftey check
        if(cache_->GetNumSegments() != numSegs)
        {
            std::cout << "number of segments found changed from "
                      << om::string::humanizeNum(cache_->GetNumSegments())
                      << " to "
                      << om::string::humanizeNum(numSegs) << "\n";
            cache_->SetNumSegments(numSegs);
        }

        recreateGUIlists_ = true;
    }

    void doUpdateSizeListsFromJoin(OmSegment* root, OmSegment* child)
    {
        SegInfo& rootInfo = list_[root->value()];
        SegInfo& childInfo = list_[child->value()];

        // failure here means tree being built out-of-order
        assert(rootInfo.sizeIncludingChildren > 0);
        assert(childInfo.sizeIncludingChildren > 0);

        rootInfo.sizeIncludingChildren += childInfo.sizeIncludingChildren;
        rootInfo.numChildren += 1 + childInfo.numChildren;

        childInfo.sizeIncludingChildren = 0;
        childInfo.numChildren = 0;

        recreateGUIlists_ = true;
    }

    void doUpdateSizeListsFromSplit(OmSegment* root, OmSegment* child,
                                    const SizeAndNumPieces childSizes)
    {
        const int64_t newChildSize = childSizes.numVoxels;
        const int32_t numPiecesInChild = childSizes.numPieces;

        assert(newChildSize > 0);
        assert(numPiecesInChild > 0);

        SegInfo& rootInfo = list_[root->value()];
        SegInfo& childInfo = list_[child->value()];

        assert(rootInfo.sizeIncludingChildren > newChildSize);
        assert(rootInfo.numChildren >= numPiecesInChild);

        assert(!childInfo.sizeIncludingChildren);
        assert(!childInfo.numChildren);

        rootInfo.sizeIncludingChildren -= newChildSize;
        rootInfo.numChildren -= numPiecesInChild;

        childInfo.sizeIncludingChildren = newChildSize;
        childInfo.numChildren = numPiecesInChild - 1; // don't include root in child count

        recreateGUIlists_ = true;
    }

    void doRecreateGUIlists(const om::common::ShouldForce force)
    {
        if(om::common::DONT_FORCE == force && !recreateGUIlists_){
            return;
        }

        OmTimer timer;

        OmThreadPool buildPool;
        buildPool.start(4);

        buildPool.push_back(
            zi::run_fn(
                zi::bind(&OmSegmentListLowLevel::copyGlobalList,
                         this)));

        buildPool.push_back(
            zi::run_fn(
                zi::bind(&OmSegmentListLowLevel::buildGUIlist,
                         this, om::common::WORKING)));

        buildPool.push_back(
            zi::run_fn(
                zi::bind(&OmSegmentListLowLevel::buildGUIlist,
                         this, om::common::VALID)));

        buildPool.push_back(
            zi::run_fn(
                zi::bind(&OmSegmentListLowLevel::buildGUIlist,
                         this, om::common::UNCERTAIN)));

        buildPool.join();

        recreateGUIlists_ = false;

        timer.Print("Rebuilt segment GUI lists");

        OmEvents::SegmentGUIlist(cache_->GetSDW(), true);
    }

    void copyGlobalList()
    {
        boost::shared_ptr<OmSegmentListGlobal> globalList =
            boost::make_shared<OmSegmentListGlobal>(list_);
        segmentLists_->Swap(globalList);
    }

    void buildGUIlist(const om::common::SegListType listType)
    {
        boost::shared_ptr<OmSegmentListForGUI> guiList =
            boost::make_shared<OmSegmentListForGUI>(listType);
        guiList->Build(list_);
        segmentLists_->Swap(guiList);
    }
};

