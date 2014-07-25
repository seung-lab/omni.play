#include "volume/build/omBuildAffinityChannel.hpp"
#include "volume/omAffinityChannel.h"
#include "volume/omAffinityGraph.h"
#include "volume/omChannel.h"
#include "utility/channelDataWrapper.hpp"

OmAffinityGraph::OmAffinityGraph() {}

// used by OmGenericManager
OmAffinityGraph::OmAffinityGraph(const om::common::ID id)
    : OmManageableObject(id) {}

OmAffinityGraph::~OmAffinityGraph() {}

void OmAffinityGraph::ImportAllChannels(const QString& hdf5fnp) {
  ImportSingleChannel(hdf5fnp, om::common::AffinityGraph::X_AFFINITY);
  ImportSingleChannel(hdf5fnp, om::common::AffinityGraph::Y_AFFINITY);
  ImportSingleChannel(hdf5fnp, om::common::AffinityGraph::Z_AFFINITY);
  OmActions::Save();
}

void OmAffinityGraph::ImportSingleChannel(const QString& hdf5fnp,
                                          const om::common::AffinityGraph aff) {
  channels_[aff] = std::make_shared<OmAffinityChannel>(aff);

  OmAffinityChannel* affChan = channels_[aff].get();

  OmBuildAffinityChannel bc(*affChan, aff);
  bc.addFileNameAndPath(hdf5fnp);
  bc.BuildBlocking();
}

OmAffinityChannel* OmAffinityGraph::GetChannel(
    const om::common::AffinityGraph aff) const {
  return channels_.find(aff)->second.get();
}