#include "handler/handler.h"

#include "volume/volume.h"

#include "utility/timer.hpp"

using namespace std;
using namespace boost;

namespace om {
namespace handler {

using namespace pipeline;

int32_t get_seg_id(volume::volume& vol, coords::global point)
{
    utility::timer t;
    int32_t segId = vol.GetSegId(point);
    cout << "get_seg_id done: " << t.s_elapsed() << " seconds" << endl;
    return segId;
}

void get_seg_ids(std::set<int32_t>& _return,
                 volume::volume& vol,
                 coords::global point,
                 const int radius,
                 const common::viewType view)
{
    utility::timer t;
    vol.GetSegIds(point, radius, view, _return);
    cout << "get_seg_ids done: " << t.s_elapsed() << " seconds" << endl;
}

}
}
