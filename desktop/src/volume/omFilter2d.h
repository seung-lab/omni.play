#pragma once

/*
 * Filter Object
 *
 * A filter can overlay a channel OR a segmentation (purcaro)
 *
 * Filters need to be refactored out of OmChannel and into their own
 *  top-level class; they should also be able to support multiple volumes,
 *  different alpha-blending levels, etc. (purcaro)
 *
 * Matthew Wimer - mwimer@mit.edu - 11/13/09
 */

#include "common/string.hpp"
#include "system/omManageableObject.h"
#include "datalayer/archive/project.h"

class OmChannel;
class OmSegmentation;

namespace om {
enum FilterType {
  OVERLAY_CHANNEL,
  OVERLAY_SEGMENTATION,
  OVERLAY_NONE
};
}  // namespace om

class OmFilter2d : public OmManageableObject {
 public:
  OmFilter2d();
  OmFilter2d(const om::common::ID);

  void Load();

  std::string GetName() { return "filter" + om::string::num(GetID()); }

  inline void SetAlpha(const double alpha) { alpha_ = alpha; }

  inline double GetAlpha() { return alpha_; }

  void SetSegmentation(const om::common::ID id);
  void SetChannel(const om::common::ID id);

  inline bool HasValidVol() const { return om::OVERLAY_NONE != filterType_; }

  inline om::FilterType FilterType() const { return filterType_; }

  inline OmChannel *GetChannel() const { return *channVolPtr_; }

  inline OmSegmentation *GetSegmentation() const { return *segVolPtr_; }

  inline om::common::ID GetChannelID() const { return chanID_; }

  inline om::common::ID GetSegmentationID() const { return segID_; }

 private:
  double alpha_;
  om::FilterType filterType_;

  boost::optional<OmChannel *> channVolPtr_;
  boost::optional<OmSegmentation *> segVolPtr_;

  om::common::ID chanID_;
  om::common::ID segID_;

  void reset();

  friend YAML::Emitter &YAML::operator<<(YAML::Emitter &, const OmFilter2d &);
  friend void YAML::operator>>(const YAML::Node &, OmFilter2d &);
  friend QDataStream &operator<<(QDataStream &, const OmFilter2d &);
  friend QDataStream &operator>>(QDataStream &, OmFilter2d &);
};
