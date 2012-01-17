#include "handler/handler.h"
#include "common/common.h"
#include "volume/volume.h"

namespace om {
namespace server {

std::ostream& operator<<(std::ostream& o, const vector3d& v) {
    return o << v.x << ", " << v.y << ", " << v.z;
}

class serverHandler : virtual public serverIf {
public:
    void add_chunk(const metadata& vol, const vector3i& chunk, const std::string& data) {
        
    }

    void delete_chunk(const metadata& vol, const vector3i& chunk){
        
    }

    void get_chunk(std::string& _return, const metadata& vol, const vector3i& chunk){
        handler::get_chunk(_return, vol, chunk);
    }

    void get_mst(std::vector<edge>& _return, const metadata& vol){
    
    }

    void get_graph(std::vector<edge>& _return, const metadata& vol){
    
    }

    void create_segmentation(metadata& _return,
                             const metadata& chan,
                             const int32_t newVolId,
                             const std::vector<std::string> & features) {
        
    }
    
    void get_chan_tile(server::tile& _return,
                       const server::metadata& vol,
                       const server::vector3d& point,
                       const server::viewType::type view) {
        handler::get_chan_tile(_return, vol, point, common::Convert(view));
    }

    void get_seg_tiles(std::map<std::string, tile> & _return,
                       const metadata& vol,
                       const int32_t segId,
                       const bbox& segBbox,
                       const server::viewType::type view) {
        handler::get_seg_tiles(_return, vol, segId, segBbox, common::Convert(view));
    }


    int32_t get_seg_id(const metadata& vol, const vector3d& point) {
        return handler::get_seg_id(vol, point);
    }

    void get_seg_ids(std::set<int32_t> & _return,
                     const metadata& vol,
                     const vector3d& point,
                     const int32_t radius,
                     const viewType::type view) {
        handler::get_seg_ids(_return, vol, point, radius, common::Convert(view));
    }

    void get_seg_data(segData& _return, const metadata& meta, const int32_t segId)
    {
        handler::get_seg_data(_return, meta, segId);
    }

    double compare_results(const std::vector<result> & old_results, const result& new_result)
    {
        return handler::compare_results(old_results, new_result);
    }

    void get_mesh(std::string& _return,
                  const std::string& uri,
                  const vector3i& chunk,
                  int32_t segId) {
        handler::get_mesh(_return, uri, chunk, segId);
    }

    void get_obj(std::string& _return,
                  const std::string& uri,
                  const vector3i& chunk,
                  int32_t segId) {
        // handler::get_obj(_return, uri, chunk, segId);
    }

    void get_seeds(std::vector<std::set<int32_t> >& _return,
                   const metadata& taskVolume,
                   const std::set<int32_t>& selected,
                   const metadata& adjacentVolume)
    {
        handler::get_seeds(_return, taskVolume, selected, adjacentVolume);
    }
};

}
}
