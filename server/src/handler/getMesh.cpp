#include <limits>
#include <zi/utility/assert.hpp>

#include "datalayer/file.h"
#include "datalayer/paths.hpp"
#include "datalayer/memMappedFile.hpp"

#include "handler/handler.h"
#include "handler/validate.hpp"

#include "common/common.h"
#include "volume/volume.h"
#include "volume/segmentation.h"
#include "utility/timer.hpp"
#include "utility/UUID.hpp"

#include "mesh/vertexIndex/data.hpp"
#include "mesh/vertexIndex/meshReader.hpp"
//#include "mesh/mesher/ziMesher.hpp"

using namespace boost;

namespace om {
namespace handler {

std::string tri_strip_to_obj(const float* points,
                             const std::size_t points_length,
                             const uint32_t* indices,
                             const std::size_t indices_length,
                             const uint32_t* strips,
                             const std::size_t strips_length) {
  std::stringstream ss;
  ZI_ASSERT(points_length % 6 == 0);

  for (auto i = 0; i < points_length; i += 6) {
    ss << "v " << points[i] << ' ' << points[i + 1] << ' ' << points[i + 2]
       << '\n';
  }

  for (auto i = 0; i < points_length; i += 6) {
    ss << "vn " << points[i + 3] << ' ' << points[i + 4] << ' ' << points[i + 5]
       << '\n';
  }

  for (auto i = 0; i < strips_length; i += 2) {
    if (strips[i] == strips[i + 1]) {
      continue;
    }

    bool even = true;
    for (auto j = strips[i]; j < strips[i] + strips[i + 1] - 2; ++j) {
      if (even) {
        ss << "f " << indices[j] + 1 << "//" << indices[j] + 1 << ' '
           << indices[j + 1] + 1 << "//" << indices[j + 1] + 1 << ' '
           << indices[j + 2] + 1 << "//" << indices[j + 2] + 1 << '\n';
      } else {
        ss << "f " << indices[j + 2] + 1 << "//" << indices[j + 2] + 1 << ' '
           << indices[j + 1] + 1 << "//" << indices[j + 1] + 1 << ' '
           << indices[j] + 1 << "//" << indices[j] + 1 << '\n';
      }
      even = !even;
    }
  }
  return ss.str();
}

void tri_strip_to_degenerate(std::vector<float>& newpoints, const float* points,
                             const std::size_t points_length,
                             const uint32_t* indices,
                             const std::size_t indices_length,
                             const uint32_t* strips,
                             const std::size_t strips_length) {
  ZI_ASSERT(points_length % 6 == 0);

  for (auto i = 0; i < strips_length; i += 2) {
    if (i > 0) {
      for (auto k = 0; k < 6; ++k) {
        newpoints.push_back(
            points[indices[strips[i - 2] + strips[i - 1] - 1] * 6 + k]);
      }

      if ((newpoints.size() / 6) % 2 == 0) {
        for (auto k = 0; k < 6; ++k) {
          newpoints.push_back(points[indices[strips[i]] * 6 + k]);
        }
      }

      for (auto k = 0; k < 6; ++k) {
        newpoints.push_back(points[indices[strips[i]] * 6 + k]);
      }
    }

    for (auto j = strips[i]; j < strips[i] + strips[i + 1]; ++j) {
      for (auto k = 0; k < 6; ++k) {
        newpoints.push_back(points[indices[j] * 6 + k]);
      }
    }

    if (i + 3 < strips_length && strips[i + 2] == strips[i + 3] &&
        strips[i + 3] == 0) {
      continue;
    }
  }
}

std::shared_ptr<mesh::VertexIndexData> loadData(coords::Chunk cc,
                                                const file::path& uri,
                                                uint32_t segId) {
  std::shared_ptr<mesh::VertexIndexData> data;

  try {
    mesh::reader reader(uri, cc);
    const mesh::DataEntry* de = reader.GetDataEntry(segId);
    if (!de || !de->wasMeshed) {
      return std::shared_ptr<mesh::VertexIndexData>();
    }

    data = reader.Read(segId);
  }
  catch (Exception e) {
    return std::shared_ptr<mesh::VertexIndexData>();
  }

  if (!data.get()) {
    return std::shared_ptr<mesh::VertexIndexData>();
  }

  if (data->TrianDataCount()) {
    return std::shared_ptr<mesh::VertexIndexData>();
  }

  if (!data->HasData()) {
    return std::shared_ptr<mesh::VertexIndexData>();
  }

  return data;
}

void get_mesh(std::string& _return, const volume::Segmentation& vol,
              const server::vector3i& chunk, int32_t mipLevel, int32_t segId) {
  coords::Chunk cc(mipLevel, chunk.x, chunk.y, chunk.z);
  auto data = loadData(cc, vol.Endpoint(), segId);

  if (!data.get()) {
    return;
  }

  std::vector<float> newVertexData;
  tri_strip_to_degenerate(newVertexData, data->VertexData(),
                          data->VertexDataCount() * 6, data->VertexIndex(),
                          data->VertexIndexCount(), data->StripData(),
                          data->StripDataCount() * 2);

  _return = std::string((char*)&newVertexData.front(),
                        newVertexData.size() * sizeof(float));
}

void get_obj(std::string& _return, const volume::Segmentation& vol,
             const server::vector3i& chunk, int32_t mipLevel, int32_t segId) {
  coords::Chunk cc(mipLevel, chunk.x, chunk.y, chunk.z);
  auto data = loadData(cc, vol.Endpoint(), segId);

  if (!data.get()) {
    return;
  }

  _return = tri_strip_to_obj(data->VertexData(), data->VertexDataCount() * 6,
                             data->VertexIndex(), data->VertexIndexCount(),
                             data->StripData(), data->StripDataCount() * 2);
}
}
}  // namespace om::handler::
