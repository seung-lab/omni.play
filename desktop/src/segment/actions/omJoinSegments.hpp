#pragma once
#include "precomp.h"

#include "utility/dataWrappers.h"
#include "segment/omSegmentUtils.hpp"
#include "segment/lists/omSegmentLists.h"
#include "segment/actions/omSetSegmentValid.hpp"
#include "events/events.h"

class OmJoinSegments {
 private:
  SegmentationDataWrapper sdw_;
  om::common::SegIDSet ids_;

 public:
  OmJoinSegments(const SegmentationDataWrapper& sdw,
                 const om::common::SegIDSet& ids)
      : sdw_(sdw), ids_(ids) {}

  om::common::SegIDSet Join() { return run(true); }

  om::common::SegIDSet UnJoin() { return run(false); }

 private:
  inline om::common::SegIDSet run(const bool join) {
    const om::common::SegIDSet ret = runOpp(join);

    sdw_.SegmentLists()->RefreshGUIlists();
    om::event::Redraw2d();
    om::event::Redraw3d();

    return ret;
  }

  inline om::common::SegIDSet runOpp(const bool join) {
    const bool anyValid = sdw_.Segments()->AreAnySegmentsInValidList(ids_);

    if (anyValid) {
      return doOppValid(join);
    }

    return doOpp(join);
  }

  inline om::common::SegIDSet doOpp(const bool join) {
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
  om::common::SegIDSet doOppValid(const bool join) {
    std::deque<OmSegment*> segPtrs;
    OmSegmentUtils::GetAllChildrenSegments(sdw_, ids_, segPtrs);

    OmSetSegmentValid validator(sdw_);
    validator.SetAsNotValidForJoin(segPtrs);

    const om::common::SegIDSet idsActuallyJoined = doOpp(join);

    validator.SetAsValidForJoin(segPtrs);

    return idsActuallyJoined;
  }
};
