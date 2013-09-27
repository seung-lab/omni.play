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
  if (!(vol.Metadata().volumeType() == common::ObjectType::SEGMENTATION)) {
    return;
  }

  auto path = vol.Endpoint() / file::Paths::Seg::MST();

  datalayer::MemMappedFile<segment::Edge> dataFile(path);
  _return.resize(dataFile.Length());

  for (auto& e : dataFile) {
    server::affinity a;
    a.a = e.node1ID;
    a.b = e.node2ID;
    a.aff = e.threshold;
    _return.push_back(a);
  }
}
}
}  // namespace om::handler::
