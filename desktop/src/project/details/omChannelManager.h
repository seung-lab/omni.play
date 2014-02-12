#pragma once

#include "common/common.h"
#include "common/genericManager.hpp"
#include "volume/omChannel.h"

namespace om {
namespace volume {
class MetadataDataSource;
}
}

extern template class om::common::GenericManager<OmChannel>;
class OmChannelManager {
 public:
  OmChannel* GetChannel(const om::common::ID id);
  OmChannel& AddChannel();

  void Load();
  void RemoveChannel(const om::common::ID id);
  bool IsChannelValid(const om::common::ID id);
  const om::common::IDSet& GetValidChannelIds();
  bool IsChannelEnabled(const om::common::ID id);
  void SetChannelEnabled(const om::common::ID id, const bool enable);
  const std::vector<OmChannel*> GetPtrVec() const;

 private:
  om::common::GenericManager<OmChannel> manager_;

  friend class YAML::convert<OmChannelManager>;
};
