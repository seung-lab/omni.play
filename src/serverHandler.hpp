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
    void get_chan_tile(server::tile& _return,
                       const server::metadata& vol,
                       const server::vector3d& point,
                       const server::viewType::type view) {
        volume::volume v = vol;
        handler::get_chan_tile(_return, v, point, common::Convert(view));
    }

    void get_seg_tiles(std::map<std::string, tile> & _return,
                       const metadata& vol,
                       const int32_t segId,
                       const bbox& segBbox,
                       const viewType::type view) {
        volume::volume v = vol;
        handler::get_seg_tiles(_return, v, segId, segBbox, common::Convert(view));
    }


    int32_t get_seg_id(const metadata& vol, const vector3d& point) {
        volume::volume v = vol;
        return handler::get_seg_id(v, point);
    }

    void get_seg_ids(std::set<int32_t> & _return,
                     const metadata& vol,
                     const vector3d& point,
                     const int32_t radius,
                     const viewType::type view) {
        volume::volume v = vol;
        handler::get_seg_ids(_return, v, point, radius, common::Convert(view));
    }

    void get_seg_bbox(bbox& _return, const metadata& vol, const int32_t segId)
    {
        volume::volume v = vol;
        handler::get_seg_bbox(_return, v, segId);
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

    void get_seeds(std::vector<std::set<int32_t> >& _return,
                   const metadata& taskVolume,
                   const std::set<int32_t>& selected,
                   const metadata& adjacentVolume)
    {
        volume::volume t = taskVolume;
        volume::volume a = adjacentVolume;
        handler::get_seeds(_return, t, selected, a);
    }
};

}
}
