#pragma once

/*
 *  Manages data structures that are shared between various parts of the system.
 * Making centralized
 *  changes in the StateManager will send events that cause the other interested
 * systems to be
 *  notified and synchronized.
 *
 *  Brett Warne - bwarne@mit.edu - 3/14/09
 */

#include "common/omCommon.h"
#include "datalayer/archive/project.h"

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
  const boost::scoped_ptr<OmChannelManager> channels_;
  const boost::scoped_ptr<OmSegmentationManager> segmentations_;
  const boost::scoped_ptr<OmAffinityGraphManager> affGraphs_;

  friend YAML::Emitter& YAML::operator<<(YAML::Emitter& out,
                                         const OmProjectVolumes& p);
  friend void YAML::operator>>(const YAML::Node& in, OmProjectVolumes& p);
  friend QDataStream& operator<<(QDataStream& out, const OmProjectVolumes& p);
  friend QDataStream& operator>>(QDataStream& in, OmProjectVolumes& p);
};
