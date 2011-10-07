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
    affinityGraph(const common::id id);
    virtual ~affinityGraph();

    void ImportAllChannels(const std::string& hdf5fnp);

    void ImportSingleChannel(const std::string& hdf5fnp,
                             const affinityGraph aff);

    chunk* MipChunk(const affinityGraph aff,
                      const coords::chunkCoord& coord);

    boost::shared_ptr<rawChunk<float> > RawChunk(const affinityGraph aff,
                                                const class coords::chunkCoord& coord);

};

