#pragma once
#include "precomp.h"

#include "volume/omFilter2d.h"
#include "project/details/omChannelManager.h"

class FilterDataWrapper {
 private:
  om::common::ID mID;
  om::common::ID mChannelID;

 public:
  FilterDataWrapper() {}

  FilterDataWrapper(const om::common::ID channelID, const om::common::ID id)
      : mID(id), mChannelID(channelID) {}

  om::common::ID getChannelID() const { return mChannelID; }

  om::common::ID GetID() const { return mID; }

  OmChannel* GetChannel() const {
    return OmProject::Volumes().Channels().GetChannel(mChannelID);
  }

  bool isValid() const {
    if (OmProject::Volumes().Channels().IsChannelValid(mChannelID)) {
      if (GetChannel()->FilterManager().IsFilterValid(mID)) {
        return true;
      }
    }

    return false;
  }

  OmFilter2d* getFilter() const {
    if (!isValid()) {
      return nullptr;
    }

    return &GetChannel()->FilterManager().GetFilter(mID);
  }

  QString getName() const {
    OmFilter2d* f = getFilter();
    if (!f) {
      return "";
    }
    return QString::fromStdString(f->GetName());
  }

  bool isEnabled() const {
    if (!isValid()) {
      return false;
    }

    return GetChannel()->FilterManager().IsFilterEnabled(mID);
  }
};
