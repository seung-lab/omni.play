
#include "utility/yaml/omYaml.hpp"
#include "volume/omAffinityGraph.h"
#include "volume/omAffinityChannel.h"
#include "volume/omChannel.h"
#include "project/details/omAffinityGraphManager.h"
#include "datalayer/archive/affinity.h"
#include "datalayer/archive/mipVolume.hpp"
#include "utility/yaml/genericManager.hpp"
#include "coordinates/yaml.hpp"

namespace YAML {

Node convert<OmAffinityGraphManager>::encode(const OmAffinityGraphManager& m) {
  return genericManager::Save(m.graphs_);
}
bool convert<OmAffinityGraphManager>::decode(const Node& node,
                                             OmAffinityGraphManager& m) {
  return genericManager::Load(node, m.graphs_);
}

Node convert<OmAffinityGraph>::encode(const OmAffinityGraph& graph) {
  Node n;
  n["X Affinity"] = *graph.GetChannel(om::common::AffinityGraph::X_AFFINITY);
  n["Y Affinity"] = *graph.GetChannel(om::common::AffinityGraph::Y_AFFINITY);
  n["Z Affinity"] = *graph.GetChannel(om::common::AffinityGraph::Z_AFFINITY);
  return n;
}

bool convert<OmAffinityGraph>::decode(const Node& node,
                                      OmAffinityGraph& graph) {

  std::shared_ptr<OmAffinityChannel> xaff(new OmAffinityChannel(
      graph.GetID(), om::common::AffinityGraph::X_AFFINITY));
  if (!convert<OmAffinityChannel>::decode(node["X Affinity"], *xaff)) {
    return false;
  }

  std::shared_ptr<OmAffinityChannel> yaff(new OmAffinityChannel(
      graph.GetID(), om::common::AffinityGraph::Y_AFFINITY));
  if (!convert<OmAffinityChannel>::decode(node["Y Affinity"], *yaff)) {
    return false;
  }

  std::shared_ptr<OmAffinityChannel> zaff(new OmAffinityChannel(
      graph.GetID(), om::common::AffinityGraph::Z_AFFINITY));
  if (!convert<OmAffinityChannel>::decode(node["Z Affinity"], *zaff)) {
    return false;
  }

  graph.channels_[om::common::AffinityGraph::X_AFFINITY] = xaff;
  graph.channels_[om::common::AffinityGraph::Y_AFFINITY] = yaff;
  graph.channels_[om::common::AffinityGraph::Z_AFFINITY] = zaff;
  return true;
}

Node convert<OmAffinityChannel>::encode(const OmAffinityChannel& chan) {
  Node n;
  om::data::archive::mipVolume<const OmAffinityChannel> volArchive(chan);
  volArchive.Store(n);

  return n;
}

bool convert<OmAffinityChannel>::decode(const Node& node,
                                        OmAffinityChannel& chan) {
  om::data::archive::mipVolume<OmAffinityChannel> volArchive(chan);
  volArchive.Load(node);
  chan.LoadVolDataIfFoldersExist();
  return true;
}

}  // namespace YAML
