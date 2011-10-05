#include "chunks/omRawChunk.hpp"
#include "volume/build/omBuildAffinityChannel.hpp"
#include "volume/affinityChannel.h"
#include "volume/affinityGraph.h"
#include "volume/channel.h"
#include "utility/channelDataWrapper.hpp"

affinityGraph::affinityGraph()
{}

// used by OmGenericManager
affinityGraph::affinityGraph(const OmID id)
    : OmManageableObject(id)
{}

affinityGraph::~affinityGraph()
{}

void affinityGraph::ImportAllChannels(const QString& hdf5fnp)
{
    ImportSingleChannel(hdf5fnp, om::X_AFFINITY);
    ImportSingleChannel(hdf5fnp, om::Y_AFFINITY);
    ImportSingleChannel(hdf5fnp, om::Z_AFFINITY);
}

void affinityGraph::ImportSingleChannel(const QString& hdf5fnp,
                                          const affinityGraph aff)
{
    ChannelDataWrapper cdw;
    channel& chan = cdw.Create();

    channels_[aff] =
        om::make_shared<affinityChannel>(&chan, aff);

    affinityChannel* affChan = channels_[aff].get();

    OmBuildAffinityChannel bc(affChan, aff);
    bc.addFileNameAndPath(hdf5fnp);
    bc.BuildBlocking();
}

// use to just read data
boost::shared_ptr<OmRawChunk<float> >
affinityGraph::RawChunk(const affinityGraph aff,
                          const om::chunkCoord& coord)
{
    affinityChannel* affChan = channels_[aff].get();

    return om::make_shared<OmRawChunk<float> >(affChan->Channel(), coord);
}

OmChunk* affinityGraph::MipChunk(const affinityGraph aff,
                                   const om::chunkCoord& coord)
{
    affinityChannel* affChan = channels_[aff].get();

    return affChan->Channel()->GetChunk(coord);
}
