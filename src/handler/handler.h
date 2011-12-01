#pragma once

#include "thrift/server.h"

namespace om {
namespace handler {

void get_chan_tile(server::tile& _return,
                   const server::metadata& vol,
                   const server::vector3d& point,
                   const server::viewType::type view);

void get_seg_tiles(std::map<std::string, server::tile> & _return,
                   const server::metadata& vol,
                   const int32_t segId,
                   const server::bbox& segBbox,
                   const server::viewType::type view);

int32_t get_seg_id(const server::metadata& vol, const server::vector3d& point);

void get_seg_ids(std::vector<int32_t> & _return,
                 const server::metadata& vol,
                 const server::vector3d& point,
                 const double radius,
                 const server::viewType::type view);

void get_seg_bbox(server::bbox& _return, const std::string&path, const int32_t segId);

double compare_results(const std::vector<server::result> & old_results,
                       const server::result& new_result);

void get_mesh(std::string& _return,
              const std::string& uri,
              const server::vector3i& chunk,
              int32_t segId);
}
}
