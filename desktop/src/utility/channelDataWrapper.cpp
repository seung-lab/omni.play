#include "utility/channelDataWrapper.hpp"
#include "volume/omFilter2dManager.h"

std::vector<OmFilter2d*> ChannelDataWrapper::GetFilters() const {
  auto c = GetChannel();
  if (c) {
    return c->FilterManager().GetFilters();
  } else {
    log_debugs(unknown) << "Invalid Channel " << id_;
    return std::vector<OmFilter2d*>();
  }
}
