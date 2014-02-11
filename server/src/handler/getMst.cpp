#include "handler/handler.h"
#include "segment/types.hpp"
#include "datalayer/memMappedFile.hpp"
#include "volume/volume.h"
#include "volume/segmentation.h"
#include "volume/metadataManager.h"
#include "datalayer/file.h"
#include "datalayer/paths.hpp"

namespace om {
namespace handler {

void get_mst(std::vector<server::affinity>& _return,
             const volume::Segmentation& vol) {
  _return.resize(vol.Edges().size());

  for (auto& e : vol.Edges()) {
    server::affinity a;
    a.a = e.node1ID;
    a.b = e.node2ID;
    a.aff = e.threshold;
    _return.push_back(a);
  }
}
}
}  // namespace om::handler::
