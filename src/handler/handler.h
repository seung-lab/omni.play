#pragma once

#include "common/common.h"
#include "thrift/server.h"

namespace om {
namespace volume { class volume; }
namespace coords { class global; class globalBbox; }
namespace handler {

void get_chunk(std::string& _return,
               const volume::volume& vol,
               const server::vector3i& chunk);

void get_mst(std::vector<server::edge>& _return, const volume::volume& vol);
void get_graph(std::vector<server::edge>& _return, const volume::volume& vol);

void get_chan_tile(server::tile& _return,
                   const volume::volume& vol,
                   const coords::global& point,
                   const common::viewType view);

void get_seg_tiles(std::map<std::string, server::tile> & _return,
                   const volume::volume& vol,
                   const int32_t segId,
                   const coords::globalBbox& segBbox,
                   const common::viewType view);

int32_t get_seg_id(const volume::volume& vol, coords::global point);

void get_seg_ids(std::set<int32_t> & _return,
                 const volume::volume& vol,
                 coords::global point,
                 const int32_t radius,
                 const common::viewType view);

void get_seg_data(server::segData& _return, const volume::volume& vol, const int32_t segId);

double compare_results(const std::vector<server::result> & old_results,
                       const server::result& new_result);

void get_mesh(std::string& _return,
              const std::string& uri,
              const server::vector3i& chunk,
              int32_t segId);

void get_obj(std::string& _return,
             const std::string& uri,
             const server::vector3i& chunk,
             int32_t segId);

void get_seeds(std::vector<std::set<int32_t> >& seedIds,
               const volume::volume& taskVolume,
               const std::set<int32_t>& selected,
               const volume::volume& adjacentVolume);

}
}
