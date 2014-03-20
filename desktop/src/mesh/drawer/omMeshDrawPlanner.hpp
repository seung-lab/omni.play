#pragma once
#include "precomp.h"

#include "chunks/omSegChunk.h"
#include "mesh/drawer/omFindChunksToDraw.hpp"
#include "mesh/drawer/omMeshPlan.h"
#include "mesh/drawer/omMeshSegmentList.hpp"
#include "segment/omSegments.h"
#include "volume/omSegmentation.h"

class OmMeshDrawPlanner {
 private:
  OmSegmentation* const segmentation_;
  OmSegments& segments_;

  OmViewGroupState& vgs_;
  const std::shared_ptr<OmVolumeCuller> culler_;
  const OmBitfield drawOptions_;
  OmMeshSegmentList* const rootSegList_;

  OmSegPtrList rootSegs_;
  std::shared_ptr<std::deque<OmSegChunk*> > chunks_;
  std::shared_ptr<OmMeshPlan> sortedSegments_;

  bool notAllSegmentsFound_;

 public:
  OmMeshDrawPlanner(OmSegmentation* segmentation, OmViewGroupState& vgs,
                    std::shared_ptr<OmVolumeCuller> culler,
                    const OmBitfield drawOptions,
                    OmMeshSegmentList* rootSegList)
      : segmentation_(segmentation),
        segments_(segmentation_->Segments()),
        vgs_(vgs),
        culler_(culler),
        drawOptions_(drawOptions),
        rootSegList_(rootSegList),
        notAllSegmentsFound_(false) {}

  std::shared_ptr<OmMeshPlan> BuildPlan(
      std::shared_ptr<std::deque<OmSegChunk*> > chunks) {
    chunks_ = chunks;
    sortedSegments_ = std::make_shared<OmMeshPlan>();
    doBuildPlan();
    return sortedSegments_;
  }

  std::shared_ptr<std::deque<OmSegChunk*> > GetChunkList() { return chunks_; }

  bool SegmentsMissing() const { return notAllSegmentsFound_; }

 private:
  inline bool checkDrawOption(const OmBitfield option) {
    return drawOptions_ & option;
  }

  void doBuildPlan() {
    findRootSegments();

    if (!rootSegs_.size()) {
      return;
    }

    if (!chunks_) {
      OmFindChunksToDraw findChunks(segmentation_, culler_.get());
      chunks_ = findChunks.FindChunksToDraw();
    }

    FOR_EACH(iter, *chunks_) { determineSegmentsToDraw(*iter); }
  }

  void findRootSegments() {
    // check to filter for relevant data values
    if (checkDrawOption(DRAWOP_SEGMENT_FILTER_SELECTED)) {
      const om::common::SegIDSet ids = segments_.GetSelectedSegmentIDs();
      FOR_EACH(iter, ids) { rootSegs_.push_back(segments_.GetSegment(*iter)); }

    } else if (checkDrawOption(DRAWOP_SEGMENT_FILTER_UNSELECTED)) {
      const om::common::SegIDSet ids = segments_.GetEnabledSegmentIDs();
      FOR_EACH(iter, ids) { rootSegs_.push_back(segments_.GetSegment(*iter)); }
    }
  }

  void determineSegmentsToDraw(OmSegChunk* chunk) {
    FOR_EACH(iter, rootSegs_) {
      OmSegment* rootSeg = (*iter);

      boost::optional<OmSegPtrList> segmentsToDraw =
          rootSegList_->GetFromCacheIfReady(chunk, rootSeg);

      if (!segmentsToDraw) {
        notAllSegmentsFound_ = true;
        continue;
      }

      if (segmentsToDraw->empty()) {
        continue;
      }

      addSegmentsToDraw(chunk->GetCoordinate(), *segmentsToDraw);
    }
  }

  void addSegmentsToDraw(const om::coords::Chunk& chunkCoord,
                         const OmSegPtrList& segmentsToDraw) {
    FOR_EACH(iter, segmentsToDraw) {
      OmSegment* seg = *iter;

      if (seg->size() < vgs_.getDustThreshold()) {
        continue;
      }

      sortedSegments_->Add(seg, chunkCoord);
    }
  }
};
