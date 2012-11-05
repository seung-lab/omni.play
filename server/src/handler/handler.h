#pragma once

#include "common/common.h"
#include "server.h"
#include "RealTimeMesher.h"

namespace om {
namespace volume { class volume; }
namespace coords { class Global; class GlobalBbox; }
namespace server { class serverHandler; }
namespace handler {

void get_chunk(std::string& _return,
               const volume::volume& vol,
               const server::vector3i& chunk);

void get_mst(std::vector<server::edge>& _return, const volume::volume& vol);
void get_graph(std::vector<server::edge>& _return, const volume::volume& vol);

void get_chan_tile(server::tile& _return,
                   const volume::volume& vol,
                   const coords::Global& point,
                   const om::common::ViewType view);

void get_seg_tiles(std::map<std::string, server::tile> & _return,
                   const volume::volume& vol,
                   const int32_t segId,
                   const coords::GlobalBbox& segBbox,
                   const om::common::ViewType view);

int32_t get_seg_id(const volume::volume& vol, coords::Global point);

void get_seg_ids(std::set<int32_t> & _return,
                 const volume::volume& vol,
                 coords::Global point,
                 const int32_t radius,
                 const om::common::ViewType view);

void get_seg_data(server::segData& _return, const volume::volume& vol, const int32_t segId);

void get_seg_list_data(std::map<int, server::segData>& _return,
                       const volume::volume& vol,
                       const std::set<int32_t>& segIds);

double compare_results(const std::vector<server::result> & old_results,
                       const server::result& new_result);

void get_mesh(std::string& _return,
              const std::string& uri,
              const server::vector3i& chunk,
              int32_t mipLevel,
              int32_t segId);

void get_remesh(std::string& _return,
                const std::string& uri,
                const server::vector3i& chunk,
                int32_t mipLevel,
                const std::set<int32_t>& segId);

void update_global_mesh(zi::mesh::RealTimeMesherIf* rtm,
                        const volume::volume& vol,
	                    const std::set<uint32_t>& segIds,
                        uint32_t segId);

void get_obj(std::string& _return,
             const std::string& uri,
             const server::vector3i& chunk,
             int32_t mipLevel,
             int32_t segId);

void get_seeds(std::vector<std::set<int32_t> >& seedIds,
               const volume::volume& taskVolume,
               const std::set<int32_t>& selected,
               const volume::volume& adjacentVolume);

}
}
