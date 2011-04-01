#ifndef OM_SEGMENT_LIST_LOW_LEVEL_HPP
#define OM_SEGMENT_LIST_LOW_LEVEL_HPP

#include "events/omEvents.h"
#include "segment/lists/omSegmentListForGUI.hpp"
#include "segment/lists/omSegmentListGlobal.hpp"
#include "segment/lists/omSegmentLists.h"
#include "segment/lowLevel/omSegmentsImplLowLevel.h"
#include "utility/omThreadPool.hpp"
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
    {
        // use threadPool as serial job queue;
        // more than 1 thread requires locking, and dealing w/ out-of-order
        //  manipulations of tree modifiations that are order-sensative
        threadPool_.start(1);
    }

    ~OmSegmentListLowLevel(){
        threadPool_.join();
    }

    void SetCache(OmSegmentsImplLowLevel* cache) {
        cache_ = cache;
    }

    void Resize(const size_t size)
    {
        threadPool_.push_back(
            zi::run_fn(
                zi::bind(&OmSegmentListLowLevel::doResize,
                         this, size)));
    }

    void BuildInitialSegmentList()
    {
        threadPool_.push_back(
            zi::run_fn(
                zi::bind(&OmSegmentListLowLevel::doBuildInitialSegmentList,
                         this)));
    }

    inline void UpdateSizeListsFromJoin(OmSegment* root, OmSegment* child)
    {
        threadPool_.push_back(
            zi::run_fn(
                zi::bind(&OmSegmentListLowLevel::doUpdateSizeListsFromJoin,
                         this, root, child)));
    }

    inline void UpdateSizeListsFromSplit(OmSegment* root, OmSegment* child,
                                         const int32_t newChildSize,
                                         const int32_t numChildren)
    {
        threadPool_.push_back(
            zi::run_fn(
                zi::bind(&OmSegmentListLowLevel::doUpdateSizeListsFromSplit,
                         this, root, child, newChildSize, numChildren)));
    }

    void AddSegment(OmSegment* seg)
    {
        threadPool_.push_back(
            zi::run_fn(
                zi::bind(&OmSegmentListLowLevel::doAddSegment,
                         this, seg)));
    }

    inline void RefreshGUIlists(){
        runRefreshGUIlists(om::DONT_FORCE);
    }

    inline void ForceRefreshGUIlists(){
        runRefreshGUIlists(om::FORCE);
    }

private:
    inline void runRefreshGUIlists(const om::ShouldForce force)
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
        const OmSegID segID = seg->value();

        if(segID >= list_.size()){
            list_.resize(segID + 10);
        }

        addSegment(seg);

        recreateGUIlists_ = true;
    }

    inline void addSegment(OmSegment* seg)
    {
        assert(seg->size() > 0);

        const SegInfo info = { seg, seg->value(), seg->size(), 0 };
        list_[seg->value()] = info;
    }

    void doBuildInitialSegmentList()
    {
        std::vector<OmSegmentPage>& pages = cache_->Pages();
        const std::set<PageNum>& validPageNums = cache_->ValidPageNums();
        const uint32_t pageSize = cache_->getPageSize();

        uint32_t numSegs = 0;

        FOR_EACH(iter, validPageNums)
        {
            for(uint32_t i = 0; i < pageSize; ++i)
            {
                OmSegmentPage& page = pages[*iter];
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
                                    const int64_t newChildSize,
                                    const int32_t numChildren)
    {
        assert(newChildSize > 0);
        assert(numChildren > 0);

        SegInfo& rootInfo = list_[root->value()];
        SegInfo& childInfo = list_[child->value()];

        assert(rootInfo.sizeIncludingChildren >= newChildSize);
        assert(rootInfo.numChildren >= numChildren);

        assert(!childInfo.sizeIncludingChildren);
        assert(!childInfo.numChildren);

        rootInfo.sizeIncludingChildren -= newChildSize;
        rootInfo.numChildren -= numChildren;

        childInfo.sizeIncludingChildren = newChildSize;
        childInfo.numChildren = numChildren - 1;

        recreateGUIlists_ = true;
    }

    void doRecreateGUIlists(const om::ShouldForce force)
    {
        if(om::DONT_FORCE == force && !recreateGUIlists_){
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
                         this, om::WORKING)));

        buildPool.push_back(
            zi::run_fn(
                zi::bind(&OmSegmentListLowLevel::buildGUIlist,
                         this, om::VALID)));

        buildPool.push_back(
            zi::run_fn(
                zi::bind(&OmSegmentListLowLevel::buildGUIlist,
                         this, om::UNCERTAIN)));

        buildPool.join();

        recreateGUIlists_ = false;

        timer.Print("Rebuilt segment GUI lists");

        OmEvents::SegmentGUIlist(cache_->GetSDW());
    }

    void copyGlobalList()
    {
        boost::shared_ptr<OmSegmentListGlobal> globalList =
            boost::make_shared<OmSegmentListGlobal>(list_);
        segmentLists_->Swap(globalList);
    }

    void buildGUIlist(const om::SegListType listType)
    {
        boost::shared_ptr<OmSegmentListForGUI> guiList =
            boost::make_shared<OmSegmentListForGUI>(listType);
        guiList->Build(list_);
        segmentLists_->Swap(guiList);
    }
};

#endif
