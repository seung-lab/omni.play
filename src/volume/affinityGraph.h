#pragma once

/**
 * TODO: serialize!! (purcaro)
 **/
#include "common/common.h"
#include "system/omManageableObject.h"

class OmChunk;
class affinityChannel;
template <typename T> class OmRawChunk;

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

    OmChunk* MipChunk(const affinityGraph aff,
                      const om::chunkCoord& coord);

    boost::shared_ptr<OmRawChunk<float> > RawChunk(const affinityGraph aff,
                                                const class om::chunkCoord& coord);

};

