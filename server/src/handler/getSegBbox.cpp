#include "handler/handler.h"
#include "handler/validate.hpp"

#include "common/common.h"
#include "datalayer/memMappedFile.hpp"
#include "utility/timer.hpp"
#include "segment/segmentTypes.hpp"
#include "volume/volume.h"

#include "boost/format.hpp"

using namespace std;
using namespace boost;

namespace om {
namespace handler {

boost::optional<server::segData> get(const volume::volume& vol,
                                     const int32_t segId) {
  boost::optional<segments::data> d = vol.GetSegmentData(segId);
  if (!d) {
    return false;
  }
  server::segData out;

  coords::dataBbox dc(d->bounds, &vol.CoordSystem(), 0);

  out.bounds = dc.toGlobalBbox();
  out.size = d->size;
  return out;
}

void get_seg_data(server::segData& _return, const volume::volume& vol,
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
                       const volume::volume& vol,
                       const std::set<int32_t>& segIds) {
  FOR_EACH(id, segIds) {
    boost::optional<server::segData> d = get(vol, *id);
    if (d) {
      _return[*id] = *d;
    }
  }
}

}
}
