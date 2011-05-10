#pragma once

/**
 * TODO: serialize!! (purcaro)
 **/

#include "system/omManageableObject.h"

class OmChunk;
class OmAffinityChannel;
class OmChunkCoord;
template <typename T> class OmRawChunk;

class OmAffinityGraph : public OmManageableObject {
private:
    std::map<om::AffinityGraph, om::shared_ptr<OmAffinityChannel> > channels_;

public:
    OmAffinityGraph();
    OmAffinityGraph(const OmID id);
    virtual ~OmAffinityGraph();

    void ImportAllChannels(const QString& hdf5fnp);

    void ImportSingleChannel(const QString& hdf5fnp,
                             const om::AffinityGraph aff);

    OmChunk* MipChunk(const om::AffinityGraph aff,
                      const OmChunkCoord& coord);

    om::shared_ptr<OmRawChunk<float> > RawChunk(const om::AffinityGraph aff,
                                                const OmChunkCoord& coord);

};

