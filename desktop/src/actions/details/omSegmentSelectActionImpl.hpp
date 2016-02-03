#pragma once
#include "precomp.h"

#include "actions/omSelectSegmentParams.hpp"
#include "common/common.h"
#include "common/string.hpp"
#include "project/omProject.h"
#include "events/events.h"
#include "utility/dataWrappers.h"
#include "utility/copyFirstN.hpp"
#include "segment/omSegments.h"
#include "segment/selection.hpp"

class OmSegmentSelectActionImpl {
 private:
  std::shared_ptr<OmSelectSegmentsParams> params_;

 public:
  OmSegmentSelectActionImpl() {}

  OmSegmentSelectActionImpl(std::shared_ptr<OmSelectSegmentsParams> params)
      : params_(params) {}

  void Execute() {
    OmSegments* segments = params_->sdw.Segments();

    segments->Selection().UpdateSegmentSelection(params_->newSelectedIDs,
                                                   params_->addToRecentList);

    om::event::SegmentModified(params_);
  }

  void Undo() {
    OmSegments* segments = params_->sdw.Segments();

    segments->Selection().UpdateSegmentSelection(params_->oldSelectedIDs,
                                                 params_->addToRecentList);

    om::event::SegmentModified(params_);
  }

  std::string Description() const {
    static const int max = 5;

    om::common::SegIDSet tempSet;
    boost::copy(params_->newSelectedIDs | boost::adaptors::map_keys,
                  std::inserter(tempSet, tempSet.begin()));
    const std::string nums =
        om::utils::MakeShortStrList<om::common::SegIDSet, om::common::SegID>(
          tempSet, max);

    std::string prefix("Selected segments: ");

    return prefix + nums;
  }

  QString classNameForLogFile() const { return "OmSegmentSelectAction"; }

 private:
  template <typename T>
  friend class OmActionLoggerThread;
  friend QDataStream& operator<<(QDataStream&,
                                 const OmSegmentSelectActionImpl&);
  friend QDataStream& operator>>(QDataStream&, OmSegmentSelectActionImpl&);

  friend QTextStream& operator<<(QTextStream& out,
                                 const OmSegmentSelectActionImpl&);
};
