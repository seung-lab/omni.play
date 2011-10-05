#pragma once

/**
 * TODO: serialize!! (purcaro)
 **/
#include "common/common.h"
#include "system/omManageableObject.h"

class chunk;
class affinityChannel;
template <typename T> class rawChunk;

class affinityGraph : public OmManageableObject {
private:
    std::map<affinityGraph, boost::shared_ptr<affinityChannel> > channels_;

public:
    affinityGraph();
    affinityGraph(const OmID id);
    virtual ~affinityGraph();

    void ImportAllChannels(const QString& hdf5fnp);

    void ImportSingleChannel(const QString& hdf5fnp,
                             const affinityGraph aff);

    chunk* MipChunk(const affinityGraph aff,
                      const om::chunkCoord& coord);

    boost::shared_ptr<rawChunk<float> > RawChunk(const affinityGraph aff,
                                                const class om::chunkCoord& coord);

};

