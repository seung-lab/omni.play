#pragma once
#include "precomp.h"

#include "events/events.h"
#include "segment/lists/omSegmentListForGUI.hpp"
#include "segment/lists/omSegmentListGlobal.hpp"
#include "segment/lists/omSegmentLists.h"
#include "segment/lowLevel/omSegmentLowLevelTypes.h"
#include "segment/lowLevel/store.h"
#include "segment/omSegments.h"
#include "threads/taskManager.hpp"
#include "utility/timer.hpp"
#include "utility/segmentationDataWrapper.hpp"
#include "common/string.hpp"
#include "volume/metadataManager.h"

class OmSegmentListsImpl {
 public:
  OmSegmentListsImpl(OmSegmentLists& lists, om::volume::MetadataManager& meta,
                     om::segment::Store& store, SegmentationDataWrapper sdw)
      : store_(store),
        meta_(meta),
        segmentLists_(lists),
        sdw_(sdw),
        recreateGUIlists_(true) {
    doResize(meta_.numSegments());
    doBuildInitialSegmentList();

    // use threadPool as serial job queue;
    // more than 1 thread requires locking, and dealing w/ out-of-order
    //  manipulations of tree modifiations that are order-sensative
    threadPool_.start(1);
  }

  ~OmSegmentListsImpl() { threadPool_.join(); }

  void Resize(const size_t size) {
    threadPool_.push_back(
        zi::run_fn(zi::bind(&OmSegmentListsImpl::doResize, this, size)));
  }

  inline void UpdateSizeListsFromJoin(OmSegment* root, OmSegment* child) {
    threadPool_.push_back(zi::run_fn(zi::bind(
        &OmSegmentListsImpl::doUpdateSizeListsFromJoin, this, root, child)));
  }

  inline void UpdateSizeListsFromSplit(OmSegment* root, OmSegment* child,
                                       const SizeAndNumPieces& childInfo) {
    threadPool_.push_back(
        zi::run_fn(zi::bind(&OmSegmentListsImpl::doUpdateSizeListsFromSplit,
                            this, root, child, childInfo)));
  }

  void AddSegment(OmSegment* seg) {
    threadPool_.push_back(
        zi::run_fn(zi::bind(&OmSegmentListsImpl::doAddSegment, this, seg)));
  }

  inline void RefreshGUIlists() { runRefreshGUIlists(false); }

  inline void ForceRefreshGUIlists() { runRefreshGUIlists(true); }

  inline int64_t GetSizeWithChildren(const om::common::SegID segID) {
    // if (segID >= list_.size()) {
    //   log_debugs << "segment " << segID << "not found";
    //   return 0;
    // }
    return list_[segID].sizeIncludingChildren;
  }

  inline int64_t GetSizeWithChildren(OmSegment* seg) {
    return GetSizeWithChildren(seg->value());
  }

 private:
  inline void runRefreshGUIlists(const bool force) {
    threadPool_.push_back(zi::run_fn(
        zi::bind(&OmSegmentListsImpl::doRecreateGUIlists, this, force)));
  }

  // avoid locking by adding to serial job list
  void doResize(const size_t size) { list_.resize(size + 1); }

  void doAddSegment(OmSegment* seg) {
    const om::common::SegID segID = seg->value();

    if (segID >= list_.size()) {
      list_.resize(segID + 10);
    }

    addSegment(seg);

    recreateGUIlists_ = true;
  }

  inline void addSegment(OmSegment* seg) {
    int64_t size = seg->size();
    if (!size) {
      size = 1;  // for newly-added segment
    }

    const SegInfo info = {seg, seg->value(), size, 0};
    list_[seg->value()] = info;
  }

  void doBuildInitialSegmentList() {
    log_debugs << "Building Initial SegmentList.";
    uint32_t numSegs = 0;
    for (auto segId = 1; segId <= meta_.numSegments(); segId++) {
      OmSegment* seg = store_.GetSegment(segId);
      if (seg && seg->value()) {
        if (seg->value() < list_.size()) {
          ++numSegs;
          addSegment(seg);
        } else {
          log_errors << "Invalid Segment " << seg->value();
        }
      }
    }

    // saftey check
    if (meta_.numSegments() != numSegs) {
      log_debugs << "number of segments found changed from "
                 << om::string::humanizeNum(meta_.numSegments()) << " to "
                 << om::string::humanizeNum(numSegs);
      meta_.set_numSegments(numSegs);
    }

    recreateGUIlists_ = true;
  }

  void doUpdateSizeListsFromJoin(OmSegment* root, OmSegment* child) {
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
                                  const SizeAndNumPieces childSizes) {
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
    childInfo.numChildren =
        numPiecesInChild - 1;  // don't include root in child count

    recreateGUIlists_ = true;
  }

  void doRecreateGUIlists(const bool force) {
    if (!force && !recreateGUIlists_) {
      return;
    }

    om::utility::timer timer;

    om::thread::ThreadPool buildPool;
    buildPool.start(4);

    buildPool.push_back(
        zi::run_fn(zi::bind(&OmSegmentListsImpl::copyGlobalList, this)));

    buildPool.push_back(
        zi::run_fn(zi::bind(&OmSegmentListsImpl::buildGUIlist, this,
                            om::common::SegListType::WORKING)));

    buildPool.push_back(
        zi::run_fn(zi::bind(&OmSegmentListsImpl::buildGUIlist, this,
                            om::common::SegListType::VALID)));

    buildPool.push_back(
        zi::run_fn(zi::bind(&OmSegmentListsImpl::buildGUIlist, this,
                            om::common::SegListType::UNCERTAIN)));

    buildPool.join();

    recreateGUIlists_ = false;

    timer.Print("Rebuilt segment GUI lists");

    om::event::SegmentGUIlist(sdw_, true);
  }

  void copyGlobalList() {
    auto globalList = std::make_shared<OmSegmentListGlobal>(list_);
    segmentLists_.Swap(globalList);
  }

  void buildGUIlist(const om::common::SegListType listType) {
    auto guiList = std::make_shared<OmSegmentListForGUI>(listType);
    guiList->Build(list_);
    segmentLists_.Swap(guiList);
  }

 private:
  om::thread::ThreadPool threadPool_;

  om::segment::Store& store_;
  om::volume::MetadataManager& meta_;
  OmSegmentLists& segmentLists_;

  SegmentationDataWrapper sdw_;

  std::vector<SegInfo> list_;
  bool recreateGUIlists_;
};
