#pragma once
#include "precomp.h"

#include "common/common.h"
#include "segment/io/omValidGroupNum.hpp"
#include "segment/lists/omSegmentLists.h"
#include "segment/omSegments.h"
#include "utility/segmentationDataWrapper.hpp"
#include "utility/timer.hpp"
#include "volume/omSegmentation.h"

class OmSetSegmentValid {
 private:
  const SegmentationDataWrapper& sdw_;
  om::segment::EdgeVector& edges_;

 public:
  OmSetSegmentValid(const SegmentationDataWrapper& sdw)
      : sdw_(sdw), edges_(sdw.GetSegmentation()->MST()) {}

  void Set(const std::set<OmSegment*>& selectedSegments, const bool valid) {
    om::utility::timer timer;

    const std::string notValid = valid ? "" : " NOT";

    log_debugs << "setting " << selectedSegments.size() << " segments as"
               << notValid << " valid...";

    doSet(selectedSegments, valid);
    sdw_.SegmentLists()->RefreshGUIlists();
    timer.PrintDone();
  }

  template <typename C>
  void SetAsNotValidForJoin(const C& segs) {
    doSet(segs, false);
  }

  template <typename C>
  void SetAsValidForJoin(const C& segs) {
    doSet(segs, true);
  }

 private:
  template <typename C>
  void doSet(const C& segs, const bool isValid) {
    auto listType = isValid ? om::common::SegListType::VALID
                            : om::common::SegListType::WORKING;

    for (OmSegment* seg : segs) {
      seg->SetListType(listType);
      setSegEdge(seg, isValid);
      std::cout << " Setting to valid: " << seg->value() << std::endl;
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
