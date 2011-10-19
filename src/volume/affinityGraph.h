#pragma once

/**
 * TODO: serialize!! (purcaro)
 **/
#include "common/common.h"
#include "common/manageableObject.hpp"

namespace om {

namespace chunks {
class chunk;
template <typename T> class rawChunk;
}

namespace volume {
class affinityChannel;

class affinityGraph : public common::manageableObject {
private:
    std::map<affinityGraph, boost::shared_ptr<affinityChannel> > channels_;

public:
    affinityGraph();
    affinityGraph(const common::id id);
    virtual ~affinityGraph();

    void ImportAllChannels(const std::string& hdf5fnp);

    void ImportSingleChannel(const std::string& hdf5fnp,
                             const affinityGraph aff);

    chunks::chunk* MipChunk(const affinityGraph aff,
                            const coords::chunkCoord& coord);

    boost::shared_ptr<chunks::rawChunk<float> > RawChunk(const affinityGraph aff,
                                                         const class coords::chunkCoord& coord);

};

} // namespace volume
} // namespace om
