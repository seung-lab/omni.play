#include "yaml-cpp/yaml.h"
#include "utility/yaml/omYaml.hpp"
#include "volume/omAffinityGraph.h"
#include "volume/omAffinityChannel.h"
#include "volume/omChannel.h"
#include "project/details/omAffinityGraphManager.h"
#include "datalayer/archive/affinity.h"
#include "utility/yaml/genericManager.hpp"
#include "utility/yaml/mipVolume.hpp"

namespace YAML {

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
  out << Key << "X Affinity" << Value << *graph.GetChannel(om::X_AFFINITY);
  out << Key << "Y Affinity" << Value << *graph.GetChannel(om::Y_AFFINITY);
  out << Key << "Z Affinity" << Value << *graph.GetChannel(om::Z_AFFINITY);
  out << EndMap;
  return out;
}

void operator>>(const Node& in, OmAffinityGraph& graph) {
  graph.channels_[om::X_AFFINITY] =
      om::make_shared<OmAffinityChannel>(graph.GetID(), om::X_AFFINITY);
  in["X Affinity"] >> *graph.GetChannel(om::X_AFFINITY);

  graph.channels_[om::Y_AFFINITY] =
      om::make_shared<OmAffinityChannel>(graph.GetID(), om::Y_AFFINITY);
  in["Y Affinity"] >> *graph.GetChannel(om::Y_AFFINITY);

  graph.channels_[om::Z_AFFINITY] =
      om::make_shared<OmAffinityChannel>(graph.GetID(), om::Z_AFFINITY);
  in["Z Affinity"] >> *graph.GetChannel(om::Z_AFFINITY);
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

}  // namespace YAML
