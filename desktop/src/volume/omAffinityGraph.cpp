#include "chunks/omRawChunk.hpp"
#include "volume/build/omBuildAffinityChannel.hpp"
#include "volume/omAffinityChannel.h"
#include "volume/omAffinityGraph.h"
#include "volume/omChannel.h"
#include "utility/channelDataWrapper.hpp"

OmAffinityGraph::OmAffinityGraph()
{}

// used by OmGenericManager
OmAffinityGraph::OmAffinityGraph(const OmID id)
    : OmManageableObject(id)
{}

OmAffinityGraph::~OmAffinityGraph()
{}

void OmAffinityGraph::ImportAllChannels(const QString& hdf5fnp)
{
    ImportSingleChannel(hdf5fnp, om::common::X_AFFINITY);
    ImportSingleChannel(hdf5fnp, om::common::Y_AFFINITY);
    ImportSingleChannel(hdf5fnp, om::common::Z_AFFINITY);
    OmActions::Save();
}

void OmAffinityGraph::ImportSingleChannel(const QString& hdf5fnp,
                                          const om::common::AffinityGraph aff)
{
    std::cout << "Building affinity channel: " << aff << std::endl;

    channels_[aff] =
        om::make_shared<OmAffinityChannel>(aff);

    OmAffinityChannel* affChan = channels_[aff].get();

    OmBuildAffinityChannel bc(affChan, aff);
    bc.addFileNameAndPath(hdf5fnp);
    bc.BuildBlocking();
}

OmAffinityChannel* OmAffinityGraph::GetChannel(const om::common::AffinityGraph aff) const
{
    return channels_.find(aff)->second.get();
}

// use to just read data
om::shared_ptr<OmRawChunk<float> >
OmAffinityGraph::RawChunk(const om::common::AffinityGraph aff,
                          const om::coords::Chunk& coord)
{
    OmAffinityChannel* affChan = channels_[aff].get();

    return om::make_shared<OmRawChunk<float> >(affChan, coord);
}

OmChunk* OmAffinityGraph::MipChunk(const om::common::AffinityGraph aff,
                                     const om::coords::Chunk& coord)
{
    OmAffinityChannel* affChan = channels_[aff].get();

    return affChan->GetChunk(coord);
}
