#pragma once

#include "events/omEvents.h"
#include "segment/io/segmentPage.hpp"
#include "segment/lists/segmentListForGUI.hpp"
#include "segment/lists/segmentListGlobal.hpp"
#include "segment/lists/segmentLists.h"
#include "segment/lowLevel/segmentsImplLowLevel.h"
#include "threads/omTaskManager.hpp"
#include "utility/omTimer.hpp"
#include "utility/segmentationDataWrapper.hpp"

class segmentListLowLevel {
private:
    OmThreadPool threadPool_;

    segmentLists *const segmentLists_;

    segmentsImplLowLevel* cache_;

    std::vector<SegInfo> list_;
    bool recreateGUIlists_;

public:
    segmentListLowLevel(segmentLists* segmentLists)
        : segmentLists_(segmentLists)
        , cache_(NULL)
        , recreateGUIlists_(true)
    {
        // use threadPool as serial job queue;
        // more than 1 thread requires locking, and dealing w/ out-of-order
        //  manipulations of tree modifiations that are order-sensative
        threadPool_.start(1);
    }

    ~segmentListLowLevel(){
        threadPool_.join();
    }

    void SetCache(segmentsImplLowLevel* cache) {
        cache_ = cache;
    }

    void Resize(const size_t size)
    {
        doResize(size);
//         threadPool_.push_back(
//             zi::run_fn(
//                 zi::bind(&segmentListLowLevel::doResize,
//                          this, size)));
    }

    void BuildInitialSegmentList()
    {
        doBuildInitialSegmentList();
//         threadPool_.push_back(
//             zi::run_fn(
//                 zi::bind(&segmentListLowLevel::doBuildInitialSegmentList,
//                          this)));
    }

    inline void UpdateSizeListsFromJoin(segment* root, segment* child)
    {
        threadPool_.push_back(
            zi::run_fn(
                zi::bind(&segmentListLowLevel::doUpdateSizeListsFromJoin,
                         this, root, child)));
    }

    inline void UpdateSizeListsFromSplit(segment* root, segment* child,
                                         const SizeAndNumPieces& childInfo)
    {
        threadPool_.push_back(
            zi::run_fn(
                zi::bind(&segmentListLowLevel::doUpdateSizeListsFromSplit,
                         this, root, child, childInfo)));
    }

    void AddSegment(segment* seg)
    {
        threadPool_.push_back(
            zi::run_fn(
                zi::bind(&segmentListLowLevel::doAddSegment,
                         this, seg)));
    }

    inline void RefreshGUIlists(){
        runRefreshGUIlists(common::DONT_FORCE);
    }

    inline void ForceRefreshGUIlists(){
        runRefreshGUIlists(om::FORCE);
    }
    
    inline int64_t GetSizeWithChildren(const segId segID) {
        if(segID >= list_.size()){
            std::cout << "segment " << segID << "not found\n";
            return 0;
        }
        return list_[segID].sizeIncludingChildren;
    }
    
    inline int64_t GetSizeWithChildren(segment* seg){
        return GetSizeWithChildren(seg->value());
    }

private:
    inline void runRefreshGUIlists(const om::ShouldForce force)
    {
        threadPool_.push_back(
            zi::run_fn(
                zi::bind(&segmentListLowLevel::doRecreateGUIlists,
                         this, force)));
    }

    // avoid locking by adding to serial job list
    void doResize(const size_t size){
        list_.resize(size);
    }

    void doAddSegment(segment* seg)
    {
        const segId segID = seg->value();

        if(segID >= list_.size()){
            list_.resize(segID + 10);
        }

        addSegment(seg);

        recreateGUIlists_ = true;
    }

    inline void addSegment(segment* seg)
    {
        uint64_t size = seg->size();
        if(!size){
            size = 1; // for newly-added segment
        }

        const SegInfo info = { seg, seg->value(), size, 0 };
        list_[seg->value()] = info;
    }

    void doBuildInitialSegmentList()
    {
        const std::vector<segmentPage*> pages = cache_->SegmentStore()->Pages();
        const uint32_t pageSize = cache_->SegmentStore()->PageSize();

        uint32_t numSegs = 0;

        FOR_EACH(iter, pages)
        {
            segmentPage* pagePtr = *iter;
            if(!pagePtr){
                continue;
            }

            segmentPage& page = *pagePtr;

            for(uint32_t i = 0; i < pageSize; ++i)
            {
                segment* seg = &(page[i]);
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

    void doUpdateSizeListsFromJoin(segment* root, segment* child)
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

    void doUpdateSizeListsFromSplit(segment* root, segment* child,
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

    void doRecreateGUIlists(const om::ShouldForce force)
    {
        if(common::DONT_FORCE == force && !recreateGUIlists_){
            return;
        }

        OmTimer timer;

        OmThreadPool buildPool;
        buildPool.start(4);

        buildPool.push_back(
            zi::run_fn(
                zi::bind(&segmentListLowLevel::copyGlobalList,
                         this)));

        buildPool.push_back(
            zi::run_fn(
                zi::bind(&segmentListLowLevel::buildGUIlist,
                         this, om::WORKING)));

        buildPool.push_back(
            zi::run_fn(
                zi::bind(&segmentListLowLevel::buildGUIlist,
                         this, om::VALID)));

        buildPool.push_back(
            zi::run_fn(
                zi::bind(&segmentListLowLevel::buildGUIlist,
                         this, om::UNCERTAIN)));

        buildPool.join();

        recreateGUIlists_ = false;

        timer.Print("Rebuilt segment GUI lists");

        OmEvents::SegmentGUIlist(cache_->GetSDW(), true);
    }

    void copyGlobalList()
    {
        boost::shared_ptr<segmentListGlobal> globalList =
            boost::make_shared<segmentListGlobal>(list_);
        segmentLists_->Swap(globalList);
    }

    void buildGUIlist(const om::SegListType listType)
    {
        boost::shared_ptr<segmentListForGUI> guiList =
            boost::make_shared<segmentListForGUI>(listType);
        guiList->Build(list_);
        segmentLists_->Swap(guiList);
    }
};

