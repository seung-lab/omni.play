#pragma once
#include "precomp.h"

#include "project/omProject.h"
#include "project/details/omProjectVolumes.h"
#include "project/details/omChannelManager.h"
#include "volume/omChannel.h"

class FilterDataWrapper;
class OmFilter2d;

class ChannelDataWrapper {
 public:
  static const om::common::IDSet& ValidIDs() {
    return OmProject::Volumes().Channels().GetValidChannelIds();
  }

  static inline const std::vector<OmChannel*> GetPtrVec() {
    return OmProject::Volumes().Channels().GetPtrVec();
  }

  static void Remove(const om::common::ID id) {
    OmProject::Volumes().Channels().RemoveChannel(id);
  }

 private:
  om::common::ID id_;

 public:
  ChannelDataWrapper() : id_(0) {}

  explicit ChannelDataWrapper(const om::common::ID ID) : id_(ID) {}

  inline om::common::ID id() const { return id_; }

  inline om::common::ID GetChannelID() const { return id_; }

  OmChannel& Create() {
    OmChannel& c = OmProject::Volumes().Channels().AddChannel();
    id_ = c.GetID();
    log_debugs << "create channel " << id_;
    return c;
  }

  void Remove() {
    Remove(id_);
    id_ = 0;
  }

  inline QString GetName() const {
    auto c = GetChannel();
    if (c) {
      return QString::fromStdString(c->GetName());
    } else {
      log_errors << "Invalid Channel: " << id_;
      return "";
    }
  }

  inline bool isEnabled() const {
    return OmProject::Volumes().Channels().IsChannelEnabled(id_);
  }

  inline OmChannel* GetChannel() const {
    return OmProject::Volumes().Channels().GetChannel(id_);
  }

  inline bool IsValidWrapper() const { return IsChannelValid(); }

  inline bool IsChannelValid() const {
    if (!id_) {
      return false;
    }
    return OmProject::Volumes().Channels().IsChannelValid(id_);
  }

  inline bool IsBuilt() const {
    auto c = GetChannel();
    if (c) {
      return c->built();
    } else {
      log_errors << "Invalid Channel: " << id_;
      return false;
    }
  }

  std::vector<OmFilter2d*> GetFilters() const;
};
