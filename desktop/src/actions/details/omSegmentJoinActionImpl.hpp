#pragma once
#include "precomp.h"

#include "utility/copyFirstN.hpp"
#include "common/common.h"
#include "segment/actions/omJoinSegments.hpp"

class OmSegmentJoinActionImpl {
 private:
  SegmentationDataWrapper sdw_;
  om::common::SegIDSet segIDs_;

 public:
  OmSegmentJoinActionImpl() {}

  OmSegmentJoinActionImpl(const SegmentationDataWrapper& sdw,
                          const om::common::SegIDSet& segIDs)
      : sdw_(sdw), segIDs_(segIDs) {}

  void Execute() {
    OmJoinSegments joiner(sdw_, segIDs_);
    segIDs_ = joiner.Join();

    OmCacheManager::TouchFreshness();
    om::event::Redraw2d();
    om::event::Redraw3d();
  }

  void Undo() {
    OmJoinSegments joiner(sdw_, segIDs_);
    segIDs_ = joiner.UnJoin();

    OmCacheManager::TouchFreshness();
    om::event::Redraw2d();
    om::event::Redraw3d();
  }

  std::string Description() {
    if (segIDs_.empty()) {
      return "did not join segments";
    }

    static const int max = 5;

    const std::string nums =
        om::utils::MakeShortStrList<om::common::SegIDSet, om::common::SegID>(
            segIDs_, max);

    return "Joined segments: " + nums;
  }

  QString classNameForLogFile() const { return "OmSegmentJoinAction"; }

 private:
  template <typename T>
  friend class OmActionLoggerThread;

  friend class QDataStream& operator<<(QDataStream&,
                                       const OmSegmentJoinActionImpl&);
  friend class QDataStream& operator>>(QDataStream&, OmSegmentJoinActionImpl&);

  friend QTextStream& operator<<(QTextStream&, const OmSegmentJoinActionImpl&);
};
