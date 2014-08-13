#include "coordinates/yaml.hpp"
#include "datalayer/archive/filter.h"
#include "project/details/omChannelManager.h"
#include "utility/yaml/genericManager.hpp"
#include "coordinates/yaml.hpp"
#include "datalayer/archive/mipVolume.hpp"
#include "utility/yaml/omBaseTypes.hpp"
#include "utility/yaml/omYaml.hpp"

namespace YAML {

Node convert<OmChannelManager>::encode(const OmChannelManager& m) {
  return genericManager::Save(m.manager_);
}
bool convert<OmChannelManager>::decode(const Node& node, OmChannelManager& m) {
  return genericManager::Load(node, m.manager_);
}

Node convert<OmChannel>::encode(const OmChannel& chan) {
  Node n;
  om::data::archive::mipVolume<const OmChannel> volArchive(chan);
  volArchive.Store(n);
  n["Filters"] = chan.filterManager_;

  return n;
}

bool convert<OmChannel>::decode(const Node& node, OmChannel& chan) {
  om::data::archive::mipVolume<OmChannel> volArchive(chan);
  volArchive.Load(node);
  chan.LoadVolDataIfFoldersExist();
  YAML::convert<OmFilter2dManager>::decode(node["Filters"],
                                           chan.filterManager_);
  return true;
}
}  // namespace YAML
