#include "segment/omSegmentUtils.hpp"
#include "common/common.h"
#include "view3d/mesh/drawer/omMeshSegmentList.hpp"
#include "view3d/mesh/drawer/omMeshSegmentListTask.h"
#include "segment/coloring.hpp"
#include "segment/omSegmentIterator.h"
#include "segment/omSegments.h"
//#include "chunk/uniqueValuesFileDataSource.hpp"

namespace om {
namespace v3d {

void MeshSegmentListTask::run() {
  const auto chunkSegIDs = chunkUniqueValues_.Get(coord_);

  if (!chunkSegIDs) {
    return;
  }

  OmSegmentIterator segIter(segments_);
  segIter.iterOverSegmentID(rootSeg_.value());
  OmSegment* seg = segIter.getNextSegment();

  SegPtrAndColorList segmentsToDraw;

  while (nullptr != seg) {
    if (chunkSegIDs->contains(seg->value())) {
      if (seg->size() < key_.dustThreshold) {
        continue;
      }
      auto color = determineColor(seg);
      segmentsToDraw.push_back({seg, color});
    }
    seg = segIter.getNextSegment();
  }

  rootSegLists_.AddToCache(coord_, rootSeg_, key_.shouldVolumeBeShownBroken,
                           key_.breakThreshold, segmentsToDraw);
  // log_debugs << "done: " << rootSeg->value;
}

Vector3f MeshSegmentListTask::determineColor(OmSegment* seg) {
  using namespace om::segment;

  coloring sccType;
  if (key_.shouldVolumeBeShownBroken) {
    sccType = coloring::SEGMENTATION_BREAK_BLACK;
  } else {
    sccType = coloring::SEGMENTATION;
  }

  if (seg->getParent() && sccType != coloring::SEGMENTATION_BREAK_BLACK) {
    return determineColor(segments_.FindRoot(seg));
  }

  if (coloring::SEGMENTATION_BREAK_BLACK != sccType) {
    return seg->GetColorFloat() * 2.;
  }

  if (!qFuzzyCompare(1, key_.breakThreshold)) {
    if (useParentColor(seg)) {
      // WARNING: recusive operation is O(depth of MST)
      auto* s =
          OmSegmentUtils::GetSegmentFromThreshold(seg, key_.breakThreshold);
      return determineColor(s);
    }
  }

  return seg->GetColorFloat();
}

bool MeshSegmentListTask::useParentColor(OmSegment* seg) {
  return seg->getParent() && seg->getThreshold() > key_.breakThreshold &&
         seg->getThreshold() < 2;
  // 2 is the manual merge threshold
}
}
}  // namespace
