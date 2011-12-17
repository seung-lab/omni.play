#include "handler/handler.h"

#include "common/common.h"
#include "datalayer/memMappedFile.hpp"
#include "utility/timer.hpp"
#include "segment/segmentTypes.hpp"
#include "volume/volume.h"

#include "boost/format.hpp"

using namespace std;
using namespace boost;

namespace om {
namespace handler {

void get_seg_bbox(server::bbox& _return, volume::volume& vol, const int32_t segId)
{
    utility::timer t;
    const segment::data& d = vol.GetSegmentData(segId);

    _return.min.x = d.bounds.getMin().x;
    _return.min.y = d.bounds.getMin().y;
    _return.min.z = d.bounds.getMin().z;
    _return.max.x = d.bounds.getMax().x;
    _return.max.y = d.bounds.getMax().y;
    _return.max.z = d.bounds.getMax().z;

    std::cout << "get_seg_bbox done: " << t.s_elapsed() << " seconds" << std::endl;
}

}
}
