#include "coordinates/yaml.hpp"
#include "datalayer/archive/filter.h"
#include "project/details/omChannelManager.h"
#include "utility/yaml/genericManager.hpp"
#include "utility/yaml/mipVolume.hpp"
#include "utility/yaml/omBaseTypes.hpp"
#include "utility/yaml/omYaml.hpp"
#include "yaml-cpp-old/yaml.h"

namespace YAMLold {

Emitter& operator<<(Emitter& out, const OmChannelManager& cm) {
  out << BeginMap;
  genericManager::Save(out, cm.manager_);
  out << EndMap;
  return out;
}

void operator>>(const Node& in, OmChannelManager& cm) {
  genericManager::Load(in, cm.manager_);
}

Emitter& operator<<(Emitter& out, const OmChannel& chan) {
  out << BeginMap;
  mipVolume<const OmChannel> volArchive(chan);
  volArchive.Store(out);

  out << Key << "Filters" << Value << chan.filterManager_;
  out << EndMap;
  return out;
}

void operator>>(const Node& in, OmChannel& chan) {
  mipVolume<OmChannel> volArchive(chan);
  volArchive.Load(in);

  in["Filters"] >> chan.filterManager_;
  chan.LoadVolDataIfFoldersExist();
}

}  // namespace YAMLold
