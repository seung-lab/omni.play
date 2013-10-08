#pragma once

#include "common/common.h"
#include "server.h"
#include "RealTimeMesher.h"

namespace om {
namespace volume {
class volume;
}
namespace coords {
class global;
class globalBbox;
}
namespace server {
class serverHandler;
}
namespace handler {

void get_tiles(std::vector<server::tile>& _return, const volume::volume& vol,
               const coords::chunk& chunk, const common::viewType view,
               const int32_t from, const int32_t to);

void get_seg_list_data(std::map<int, server::segData>& _return,
                       const volume::volume& vol,
                       const std::set<int32_t>& segIds);

bool modify_global_mesh_data(zi::mesh::RealTimeMesherIf* rtm,
                             const volume::volume& vol,
                             const std::set<uint32_t> addedSegIds,
                             const std::set<uint32_t> modifiedSegIds,
                             int32_t segId);

void get_mesh(std::string& _return, const volume::volume& vol,
              const server::vector3i& chunk, int32_t mipLevel, int32_t segId);

void get_obj(std::string& _return, const volume::volume& vol,
             const server::vector3i& chunk, int32_t mipLevel, int32_t segId);

void get_seeds(std::vector<std::map<int32_t, int32_t> >& seedIds,
               const volume::volume& taskVolume,
               const std::set<int32_t>& selected,
               const volume::volume& adjacentVolume);

}
}
