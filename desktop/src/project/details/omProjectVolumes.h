#pragma once

#include "common/common.h"

class OmAffinityGraphManager;
class OmChannelManager;
class OmSegmentationManager;

class OmProjectVolumes {
 public:
  OmProjectVolumes();
  ~OmProjectVolumes();

  OmChannelManager& Channels() { return *channels_; }

  const OmChannelManager& Channels() const { return *channels_; }

  OmSegmentationManager& Segmentations() { return *segmentations_; }

  const OmSegmentationManager& Segmentations() const { return *segmentations_; }

  OmAffinityGraphManager& AffinityGraphs() { return *affGraphs_; }

  const OmAffinityGraphManager& AffinityGraphs() const { return *affGraphs_; }

 private:
  const std::unique_ptr<OmChannelManager> channels_;
  const std::unique_ptr<OmSegmentationManager> segmentations_;
  const std::unique_ptr<OmAffinityGraphManager> affGraphs_;
};
