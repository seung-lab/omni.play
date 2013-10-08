#pragma once

#include "utility/dataWrappers.h"
#include "segment/omSegmentUtils.hpp"
#include "segment/lists/omSegmentLists.h"
#include "segment/actions/omSetSegmentValid.hpp"
#include "events/omEvents.h"

class OmJoinSegments {
 private:
  SegmentationDataWrapper sdw_;
  OmSegIDsSet ids_;

 public:
  OmJoinSegments(const SegmentationDataWrapper& sdw, const OmSegIDsSet& ids)
      : sdw_(sdw), ids_(ids) {}

  OmSegIDsSet Join() { return run(true); }

  OmSegIDsSet UnJoin() { return run(false); }

 private:
  inline OmSegIDsSet run(const bool join) {
    const OmSegIDsSet ret = runOpp(join);

    sdw_.SegmentLists()->RefreshGUIlists();
    OmEvents::Redraw2d();
    OmEvents::Redraw3d();

    return ret;
  }

  inline OmSegIDsSet runOpp(const bool join) {
    const bool anyValid = sdw_.Segments()->AreAnySegmentsInValidList(ids_);

    if (anyValid) {
      return doOppValid(join);
    }

    return doOpp(join);
  }

  inline OmSegIDsSet doOpp(const bool join) {
    if (join) {
      return sdw_.Segments()->JoinTheseSegments(ids_);
    }
    return sdw_.Segments()->UnJoinTheseSegments(ids_);
  }

  /*
   * if the join involves validated segments:
   *  1.) find all child segments across all segments involved in the join
   *  2.) invalidate all segments
   *  3.) perform the join
   *  4.) (re)validate all segments
   */
  OmSegIDsSet doOppValid(const bool join) {
    std::deque<OmSegment*> segPtrs;
    OmSegmentUtils::GetAllChildrenSegments(sdw_, ids_, segPtrs);

    OmSetSegmentValid validator(sdw_);
    validator.SetAsNotValidForJoin(segPtrs);

    const OmSegIDsSet idsActuallyJoined = doOpp(join);

    validator.SetAsValidForJoin(segPtrs);

    return idsActuallyJoined;
  }
};
