#pragma once
#include "precomp.h"

#include "actions/io/omActionLogger.hpp"
#include "common/common.h"
#include "common/string.hpp"
#include "events/events.h"
#include "gui/sidebars/right/rightImpl.h"
#include "system/omAppState.hpp"
#include "utility/segmentationDataWrapper.hpp"

class OmSegmentationSizeThresholdChangeActionImpl {
 private:
  SegmentationDataWrapper sdw_;
  double threshold_;
  double oldThreshold_;

 public:
  OmSegmentationSizeThresholdChangeActionImpl() {}

  OmSegmentationSizeThresholdChangeActionImpl(const SegmentationDataWrapper sdw,
                                              const double threshold)
      : sdw_(sdw), threshold_(threshold) {}

  void Execute() {
    if (!sdw_.IsSegmentationValid()) {
      throw om::ArgException(
          "Invalid SegmentationDataWrapper "
          "(OmSegmentationSizeThresholdChangeActionImpl::Execute)");
    }
    OmSegmentation& seg = sdw_.GetSegmentation();
    oldThreshold_ = seg.GetSizeThreshold();
    seg.SetSizeThreshold(threshold_);

    om::event::RefreshMSTthreshold();
    om::event::SegmentModified();
  }

  void Undo() {
    if (!sdw_.IsSegmentationValid()) {
      throw om::ArgException(
          "Invalid SegmentationDataWrapper "
          "(OmSegmentationSizeThresholdChangeActionImpl::Undo)");
    }
    OmSegmentation& seg = sdw_.GetSegmentation();
    seg.SetSizeThreshold(oldThreshold_);

    om::event::RefreshMSTthreshold();
    om::event::SegmentModified();
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
      QDataStream&, const OmSegmentationSizeThresholdChangeActionImpl&);
  friend class QDataStream& operator>>(
      QDataStream&, OmSegmentationSizeThresholdChangeActionImpl&);
  friend class QTextStream& operator<<(
      QTextStream& out, const OmSegmentationSizeThresholdChangeActionImpl& a);
};
