#include "chunks/rawChunk.hpp"
#include "volume/build/omBuildAffinityChannel.hpp"
#include "volume/affinityChannel.h"
#include "volume/affinityGraph.h"
#include "volume/channel.h"
#include "utility/channelDataWrapper.hpp"

affinityGraph::affinityGraph()
{}

// used by genericManager
affinityGraph::affinityGraph(const common::id id)
    : OmManageableObject(id)
{}

affinityGraph::~affinityGraph()
{}

void affinityGraph::ImportAllChannels(const std::string& hdf5fnp)
{
    ImportSingleChannel(hdf5fnp, om::X_AFFINITY);
    ImportSingleChannel(hdf5fnp, om::Y_AFFINITY);
    ImportSingleChannel(hdf5fnp, om::Z_AFFINITY);
}

void affinityGraph::ImportSingleChannel(const std::string& hdf5fnp,
                                          const affinityGraph aff)
{
    ChannelDataWrapper cdw;
    channel& chan = cdw.Create();

    channels_[aff] =
        boost::make_shared<affinityChannel>(&chan, aff);

    affinityChannel* affChan = channels_[aff].get();

    OmBuildAffinityChannel bc(affChan, aff);
    bc.addFileNameAndPath(hdf5fnp);
    bc.BuildBlocking();
}

// use to just read data
boost::shared_ptr<rawChunk<float> >
affinityGraph::RawChunk(const affinityGraph aff,
                          const coords::chunkCoord& coord)
{
    affinityChannel* affChan = channels_[aff].get();

    return boost::make_shared<rawChunk<float> >(affChan->Channel(), coord);
}

chunk* affinityGraph::MipChunk(const affinityGraph aff,
                                   const coords::chunkCoord& coord)
{
    affinityChannel* affChan = channels_[aff].get();

    return affChan->Channel()->GetChunk(coord);
}
