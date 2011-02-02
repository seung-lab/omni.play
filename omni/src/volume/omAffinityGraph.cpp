#include "project/omChannelManager.h"
#include "project/omProject.h"
#include "project/omProjectVolumes.h"
#include "volume/build/omBuildAffinityChannel.hpp"
#include "volume/omAffinityChannel.h"
#include "volume/omAffinityGraph.h"
#include "volume/omChannel.h"
#include "volume/omRawChunk.hpp"

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
	ImportSingleChannel(hdf5fnp, om::X_AFFINITY);
	ImportSingleChannel(hdf5fnp, om::Y_AFFINITY);
	ImportSingleChannel(hdf5fnp, om::Z_AFFINITY);
}

void OmAffinityGraph::ImportSingleChannel(const QString& hdf5fnp,
										  const om::AffinityGraph aff)
{
	OmChannel& chan = OmProject::Volumes().Channels().AddChannel();

	channels_[aff] =
		boost::make_shared<OmAffinityChannel>(&chan, aff);

	OmAffinityChannel* affChan = channels_[aff].get();

	OmBuildAffinityChannel bc(affChan, aff);
	bc.addFileNameAndPath(hdf5fnp);
	bc.BuildBlocking();
}

// use to just read data
boost::shared_ptr<OmRawChunk<float> >
OmAffinityGraph::RawChunk(const om::AffinityGraph aff,
						  const OmChunkCoord& coord)
{
	OmAffinityChannel* affChan = channels_[aff].get();

	return boost::make_shared<OmRawChunk<float> >(affChan->Channel(), coord);
}

OmChunkPtr OmAffinityGraph::MipChunk(const om::AffinityGraph aff,
										const OmChunkCoord& coord)
{
	OmAffinityChannel* affChan = channels_[aff].get();

	OmChunkPtr ret;
	affChan->Channel()->GetChunk(ret, coord);

	return ret;
}
