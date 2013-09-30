#include "utility/channelDataWrapper.hpp"
#include "utility/dataWrappers.h"

std::vector<OmFilter2d*> ChannelDataWrapper::GetFilters() const {
  return GetChannel().FilterManager().GetFilters();
}
