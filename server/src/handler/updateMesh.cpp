#include "RealTimeMesher.h"
#include "chunk/cachedDataSource.hpp"
#include "chunk/uniqueValuesFileDataSource.hpp"
#include "common/string.hpp"
#include "handler/handler.h"
#include "pipeline/filter.hpp"
#include "pipeline/unchunk.hpp"
#include "rtm_constants.h"
#include "tile/cachedDataSource.hpp"
#include "volume/metadataManager.h"
#include "volume/segmentation.h"
#include "volume/volume.h"

#include <boost/multi_array.hpp>

#include <algorithm>

namespace om {
namespace handler {

typedef boost::shared_ptr<zi::mesh::RealTimeMesherIf> MesherPtr;
typedef std::function<MesherPtr()> ConnectionFunc;

class Sender : public boost::static_visitor<> {
 public:
  Sender(const volume::Segmentation& v, const coords::Data& min,
         const coords::Data& max, ConnectionFunc connect,
         const std::set<uint32_t> addedSegIds,
         const std::set<uint32_t> modifiedSegIds, int32_t segId)
      : v_(v),
        min_(min),
        max_(max),
        connect_(connect),
        addedSegIds_(addedSegIds),
        modifiedSegIds_(modifiedSegIds),
        segId_(segId) {}

  template <typename T>
  void operator()(chunk::Chunk<T> in) const {
    using namespace boost;
    typedef multi_array_ref<T, 3> array;
    typedef typename array::template array_view<3>::type array_view;
    typedef multi_array_types::index_range range;

    array chunkData(&in[0], extents[128][128][128], fortran_storage_order());

    Vector3i localMin;
    localMin.x = min_.x % 128;
    localMin.y = min_.y % 128;
    localMin.z = min_.z % 128;

    if (max_.x <= localMin.x || max_.y <= localMin.y || max_.z <= localMin.z) {
      return;
    }

    auto location = min_.ToGlobal();

    const typename array::size_type* shape = chunkData.shape();
    size_t length = shape[0] * shape[1] * shape[2];

    std::string out(length * sizeof(T), 0);
    multi_array_ref<T, 3> out_ref(
        reinterpret_cast<T*>(const_cast<char*>(out.data())),
        extents[shape[0]][shape[1]][shape[2]], fortran_storage_order());

    std::string mask(length, 0);
    multi_array_ref<char, 3> mask_ref(const_cast<char*>(mask.data()),
                                      extents[shape[0]][shape[1]][shape[2]],
                                      fortran_storage_order());

    for (auto x = 0; x < shape[0]; ++x) {
      for (auto y = 0; y < shape[1]; ++y) {
        for (auto z = 0; z < shape[2]; ++z) {
          if (addedSegIds_.count(chunkData[x][y][z])) {
            out_ref[x][y][z] = segId_;
          }
          if (localMin.x <= x && x <= max_.x && localMin.y <= y &&
              y <= max_.y && localMin.z <= z && z <= max_.z &&
              modifiedSegIds_.count(chunkData[x][y][z])) {
            mask_ref[x][y][z] = 1;
          }
        }
      }
    }

    zi::mesh::Vector3i loc, size;
    loc.x = location.x;
    loc.y = location.y;
    loc.z = location.z;
    size.x = shape[0];
    size.y = shape[1];
    size.z = shape[2];

    auto rtm = connect_();
    bool succeded = false;
    do {
      try {
        log_debugs << "Sending: " << segId_ << " - " << location << " | ["
                   << size.x << ", " << size.y << ", " << size.z << "] "
                   << out.size() << " bytes.";
        rtm->maskedUpdate(string::num(segId_), loc, size, out, mask);
        succeded = true;
      }
      catch (apache::thrift::TException& tx) {
        log_debugs << "Unable to update RTM: " << tx.what();
        sleep(1000);
        rtm = connect_();
      }
    } while (!succeded);
  }

 private:
  const volume::Segmentation& v_;
  const coords::Data& min_;
  const coords::Data& max_;
  ConnectionFunc connect_;
  const std::set<uint32_t> addedSegIds_;
  const std::set<uint32_t> modifiedSegIds_;
  int32_t segId_;
};

bool modified(std::shared_ptr<chunk::UniqueValues> uv,
              const std::set<uint32_t> modifiedSegIds) {
  if (!uv || !uv->Values.size()) {
    return false;
  }

  std::vector<uint32_t> intersection(modifiedSegIds.size());
  auto it = std::set_intersection(uv->Values.begin(), uv->Values.end(),
                                  modifiedSegIds.begin(), modifiedSegIds.end(),
                                  intersection.begin());

  return it != intersection.begin();
}

bool modify_global_mesh_data(ConnectionFunc c, const volume::Segmentation& vol,
                             const std::set<uint32_t> addedSegIds,
                             const std::set<uint32_t> modifiedSegIds,
                             int32_t segId) {
  if (vol.Metadata().volumeType() != common::ObjectType::SEGMENTATION) {
    throw ArgException("Can only update global mesh from segmentation");
  }

  using namespace boost;

  for (auto& cc : *vol.Coords().MipChunkCoords(0)) {
    if (!modified(vol.UniqueValuesDS().Get(cc), modifiedSegIds)) {
      continue;
    }

    auto chunk = vol.ChunkDS().Get(cc);

    coords::DataBbox bounds = cc.BoundingBox(vol.Coords());

    coords::Data min = bounds.getMin();
    coords::Data max = bounds.getMax();
    Vector3i volMax = vol.Coords().DataDimensions();

    const int TRIM = 2;
    max.x = std::min(max.x, volMax.x - TRIM) - min.x;
    max.y = std::min(max.y, volMax.y - TRIM) - min.y;
    max.z = std::min(max.z, volMax.z - TRIM) - min.z;

    min.x = std::max(min.x, TRIM);
    min.y = std::max(min.y, TRIM);
    min.z = std::max(min.z, TRIM);

    boost::apply_visitor(
        Sender(vol, min, max, c, addedSegIds, modifiedSegIds, segId),
        *vol.ChunkDS().Get(cc));
  }
  return true;
}
}
}  // namespace om::handler::
