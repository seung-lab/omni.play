#include "handler/handler.h"
#include "handler/validate.hpp"

#include "common/common.h"
#include "datalayer/memMappedFile.hpp"
#include "utility/timer.hpp"
#include "utility/convert.hpp"
#include "segment/types.hpp"
#include "volume/volume.h"
#include "volume/segmentation.h"
#include "volume/metadataManager.h"
#include "datalayer/pagedVector.hpp"

#include "boost/format.hpp"

using namespace boost;

namespace om {
namespace handler {

boost::optional<server::segData> get(const volume::Segmentation& vol,
                                     const int32_t segId) {
  auto& pg = vol.SegData();
  try {
    auto d = pg[segId];
    server::segData out;

    coords::DataBbox dc(d.bounds, vol.Coords(), 0);

    out.bounds = utility::Convert(dc.ToGlobalBbox());
    out.size = d.size;
    return out;
  }
  catch (Exception e) {
    return boost::optional<server::segData>();
  }
}

void get_seg_data(server::segData& _return, const volume::Segmentation& vol,
                  const int32_t segId) {
  boost::optional<server::segData> d = get(vol, segId);
  if (!d) {
    return;
  }
  _return = *d;
}

std::ostream& operator<<(std::ostream& out, const server::vector3d& vec) {
  return out << "[" << vec.x << "\t" << vec.y << "\t" << vec.z << "]";
}

std::ostream& operator<<(std::ostream& out, const server::bbox& bounds) {
  out << "min " << bounds.min;
  return out << " max " << bounds.max;
}

std::ostream& operator<<(std::ostream& out, const server::segData& data) {
  out << "bounds: " << data.bounds << std::endl;
  return out << "size: " << data.size << std::endl;
}

void get_seg_list_data(std::map<int32_t, server::segData>& _return,
                       const volume::Segmentation& vol,
                       const std::set<int32_t>& segIds) {
  for (auto& id : segIds) {
    boost::optional<server::segData> d = get(vol, id);
    if (d) {
      _return[id] = *d;
    }
  }
}
}
}
