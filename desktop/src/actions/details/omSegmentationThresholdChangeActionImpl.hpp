#pragma once
#include "precomp.h"

#include "actions/io/omActionLogger.hpp"
#include "common/common.h"
#include "common/string.hpp"
#include "events/events.h"
#include "gui/sidebars/right/rightImpl.h"
#include "system/omAppState.hpp"
#include "utility/segmentationDataWrapper.hpp"
#include "volume/omSegmentation.h"

class OmSegmentationThresholdChangeActionImpl {
 private:
  SegmentationDataWrapper sdw_;
  float threshold_;
  float oldThreshold_;

 public:
  OmSegmentationThresholdChangeActionImpl() {}

  OmSegmentationThresholdChangeActionImpl(const SegmentationDataWrapper sdw,
                                          const double threshold)
      : sdw_(sdw), threshold_(threshold) {}

  void Execute() {
    if (!sdw_.IsSegmentationValid()) {
      throw om::ArgException(
          "Invalid SegmentationDataWrapper "
          "(OmSegmentationThresholdChangeActionImpl::Execute)");
    }
    OmSegmentation& seg = *sdw_.GetSegmentation();
    oldThreshold_ = seg.GetDendThreshold();
    seg.SetDendThreshold(threshold_);
    om::event::RefreshMSTthreshold();
  }

  void Undo() {
    if (!sdw_.IsSegmentationValid()) {
      throw om::ArgException(
          "Invalid SegmentationDataWrapper "
          "(OmSegmentationThresholdChangeActionImpl::Undo)");
    }
    OmSegmentation& seg = *sdw_.GetSegmentation();

    seg.SetDendThreshold(oldThreshold_);

    om::event::RefreshMSTthreshold();
  }

  std::string Description() const {
    return "Threshold: " + om::string::num(threshold_);
  }

  QString classNameForLogFile() const {
    return "OmSegmentationThresholdChangeAction";
  }

 private:
  template <typename T>
  friend class OmActionLoggerThread;

  friend class QDataStream& operator<<(
      QDataStream&, const OmSegmentationThresholdChangeActionImpl&);
  friend class QDataStream& operator>>(
      QDataStream&, OmSegmentationThresholdChangeActionImpl&);
  friend class QTextStream& operator<<(
      QTextStream& out, const OmSegmentationThresholdChangeActionImpl& a);
};
