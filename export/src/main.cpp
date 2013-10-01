#include <zi/arguments.hpp>

#include "objExporter.hpp"
#include "rawExporter.hpp"
#include "mesh/vertexIndex/dataSource.h"
#include "chunk/uniqueValuesFileDataSource.hpp"
#include "volume/segmentation.h"
#include "common/logging.h"

ZiARG_string(path, "", "Project Path.");
ZiARG_int32(segId, 0, "SegID to export");
ZiARG_int32(mip, 0, "mip level to export");
ZiARG_bool(obj, false, "Export obj");
ZiARG_uint32_list(resolution, "Resolution Adjustment. (must be 3 elements)");
ZiARG_double(scale, 1, "Scale factor for the output vales");

using namespace om;

int main(int argc, char* argv[]) {
  zi::parse_arguments(argc, argv, true);

  logging::initLogging("", false);

  file::Paths p(ZiARG_path);
  volume::Volume v(p.Segmentation(1));

  auto coords = v.Coords();
  if (ZiARG_resolution.size() >= 3) {
    coords.SetResolution(coords.Resolution() * Vector3i(ZiARG_resolution[0],
                                                        ZiARG_resolution[1],
                                                        ZiARG_resolution[2]));
  }

  chunk::UniqueValuesFileDataSource uvfds(p.Segmentation(1));
  mesh::VertexIndexDataSource meshDS(p.Segmentation(1), &uvfds);

  std::unique_ptr<exporter::IMeshExporter> exporter;
  if (ZiARG_obj) {
    exporter.reset(new exporter::Obj(coords, ZiARG_scale));
  } else {
    exporter.reset(new exporter::Raw());
  }

  auto chunks = coords.MipChunkCoords(ZiARG_mip);
  for (auto& chunk : *chunks) {
    auto mesh = meshDS.Get(coords::Mesh(chunk, ZiARG_segId));
    if (mesh && mesh->HasData()) {
      exporter->Accumulate(
          mesh->Data().VertexData(), mesh->Data().VertexDataCount() * 6,
          mesh->Data().VertexIndex(), mesh->Data().VertexIndexCount(),
          mesh->Data().StripData(), mesh->Data().StripDataCount() * 2);
    }
  }

  std::cout << exporter->Write() << std::endl;

  return 0;
}