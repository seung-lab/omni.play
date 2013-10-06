#include "yaml-cpp-old/yaml.h"
#include "volume/omAffinityGraph.h"
#include "volume/omAffinityChannel.h"
#include "volume/omChannel.h"
#include "project/details/omAffinityGraphManager.h"
#include "datalayer/archive/affinity.h"
#include "utility/yaml/genericManager.hpp"
#include "utility/yaml/mipVolume.hpp"

#include "common_yaml.hpp"

namespace YAMLold {

Emitter& operator<<(Emitter& out, const OmAffinityGraphManager& cm) {
  out << BeginMap;
  genericManager::Save(out, cm.graphs_);
  out << EndMap;
  return out;
}

void operator>>(const Node& in, OmAffinityGraphManager& cm) {
  genericManager::Load(in, cm.graphs_);
}

Emitter& operator<<(Emitter& out, const OmAffinityGraph& graph) {
  out << BeginMap;
  out << Key << "X Affinity" << Value
      << *graph.GetChannel(om::common::AffinityGraph::X_AFFINITY);
  out << Key << "Y Affinity" << Value
      << *graph.GetChannel(om::common::AffinityGraph::Y_AFFINITY);
  out << Key << "Z Affinity" << Value
      << *graph.GetChannel(om::common::AffinityGraph::Z_AFFINITY);
  out << EndMap;
  return out;
}

void operator>>(const Node& in, OmAffinityGraph& graph) {
  graph.channels_[om::common::AffinityGraph::X_AFFINITY] =
      std::make_shared<OmAffinityChannel>(
          graph.GetID(), om::common::AffinityGraph::X_AFFINITY);
  in["X Affinity"] >> *graph.GetChannel(om::common::AffinityGraph::X_AFFINITY);

  graph.channels_[om::common::AffinityGraph::Y_AFFINITY] =
      std::make_shared<OmAffinityChannel>(
          graph.GetID(), om::common::AffinityGraph::Y_AFFINITY);
  in["Y Affinity"] >> *graph.GetChannel(om::common::AffinityGraph::Y_AFFINITY);

  graph.channels_[om::common::AffinityGraph::Z_AFFINITY] =
      std::make_shared<OmAffinityChannel>(
          graph.GetID(), om::common::AffinityGraph::Z_AFFINITY);
  in["Z Affinity"] >> *graph.GetChannel(om::common::AffinityGraph::Z_AFFINITY);
}

Emitter& operator<<(Emitter& out, const OmAffinityChannel& c) {
  out << BeginMap;
  mipVolume<const OmAffinityChannel> volArchive(c);
  volArchive.Store(out);
  out << EndMap;
  return out;
}

void operator>>(const Node& in, OmAffinityChannel& c) {
  mipVolume<OmAffinityChannel> volArchive(c);
  volArchive.Load(in);

  c.LoadVolDataIfFoldersExist();
}

}  // namespace YAMLold
