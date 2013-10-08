#pragma once

#include "common/omCommon.h"
#include "segment/io/omMST.h"
#include "segment/io/omValidGroupNum.hpp"
#include "segment/lists/omSegmentLists.h"
#include "segment/omSegments.h"
#include "utility/dataWrappers.h"
#include "utility/omTimer.hpp"
#include "volume/omSegmentation.h"

class OmSetSegmentValid {
 private:
  const SegmentationDataWrapper& sdw_;
  OmMSTEdge* const edges_;

 public:
  OmSetSegmentValid(const SegmentationDataWrapper& sdw)
      : sdw_(sdw), edges_(sdw.MST()->Edges()) {}

  void Set(const std::set<OmSegment*>& selectedSegments, const bool valid) {
    OmTimer timer;

    const std::string notValid = valid ? "" : " NOT";

    std::cout << "setting " << selectedSegments.size() << " segments as"
              << notValid << " valid..." << std::flush;

    doSet(selectedSegments, valid);

    sdw_.SegmentLists()->RefreshGUIlists();

    timer.PrintDone();
  }

  template <typename C> void SetAsNotValidForJoin(const C& segs) {
    doSet(segs, false);
  }

  template <typename C> void SetAsValidForJoin(const C& segs) {
    doSet(segs, true);
  }

 private:
  template <typename C> void doSet(const C& segs, const bool isValid) {
    const om::SegListType listType = isValid ? om::VALID : om::WORKING;

    FOR_EACH(iter, segs) {
      OmSegment* seg = *iter;
      seg->SetListType(listType);
      setSegEdge(seg, isValid);
    }

    sdw_.ValidGroupNum()->Set(segs, isValid);
  }

  inline void setSegEdge(OmSegment* seg, const bool valid) {
    const int edgeNum = seg->getEdgeNumber();

    if (-1 == edgeNum) {
      return;
    }

    // force edge to be joined on MST load
    edges_[edgeNum].userJoin = valid;
  }
};
