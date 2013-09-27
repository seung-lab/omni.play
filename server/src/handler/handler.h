#pragma once

#include "common/common.h"
#include "coordinates/coordinates.h"
#include "server.h"
#include "RealTimeMesher.h"

namespace om {
namespace volume {
class Volume;
class Segmentation;
}
namespace coords {
class Global;
class GlobalBbox;
}
namespace server {
class serverHandler;
}
namespace handler {

void get_tiles(std::vector<server::tile>& _return, const volume::Volume& vol,
               const coords::Chunk& chunk, const common::ViewType view,
               const int32_t from, const int32_t to);

void get_seg_list_data(std::map<int, server::segData>& _return,
                       const volume::Segmentation& vol,
                       const std::set<int32_t>& segIds);

bool modify_global_mesh_data(
    std::function<boost::shared_ptr<zi::mesh::RealTimeMesherIf>()> c,
    const volume::Segmentation& vol, const std::set<uint32_t> addedSegIds,
    const std::set<uint32_t> modifiedSegIds, int32_t segId);

void get_mesh(std::string& _return, const volume::Segmentation& vol,
              const server::vector3i& chunk, int32_t mipLevel, int32_t segId);

void get_obj(std::string& _return, const volume::Segmentation& vol,
             const server::vector3i& chunk, int32_t mipLevel, int32_t segId);

void get_seeds(std::vector<std::map<int32_t, int32_t>>& seedIds,
               const volume::Segmentation& taskVolume,
               const std::set<int32_t>& selected,
               const volume::Segmentation& adjacentVolume);

void get_mst(std::vector<server::affinity>& _return,
             const volume::Segmentation& vol);
}
}
