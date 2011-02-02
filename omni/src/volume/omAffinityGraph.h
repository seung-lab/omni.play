#ifndef OM_AFFINITY_GRAPH_H
#define OM_AFFINITY_GRAPH_H

/**
 * TODO: serialize!! (purcaro)
 **/

#include "system/omManageableObject.h"
#include "chunks/omChunkTypes.hpp"

class OmAffinityChannel;
class OmChunkCoord;
template <typename T> class OmRawChunk;

class OmAffinityGraph : public OmManageableObject {
private:
	std::map<om::AffinityGraph, boost::shared_ptr<OmAffinityChannel> > channels_;

public:
	OmAffinityGraph();
	OmAffinityGraph(const OmID id);
	virtual ~OmAffinityGraph();

	void ImportAllChannels(const QString& hdf5fnp);

	void ImportSingleChannel(const QString& hdf5fnp,
							 const om::AffinityGraph aff);

	OmChunkPtr MipChunk(const om::AffinityGraph aff,
						const OmChunkCoord& coord);

	boost::shared_ptr<OmRawChunk<float> > RawChunk(const om::AffinityGraph aff,
												   const OmChunkCoord& coord);

};

#endif
