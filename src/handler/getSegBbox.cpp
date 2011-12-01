#include "handler/handler.h"
#include "handler/validate.hpp"

#include "common/common.h"
#include "datalayer/memMappedFile.hpp"
#include "utility/timer.hpp"
#include "segment/segmentTypes.hpp"

#include "boost/format.hpp"

using namespace std;
using namespace boost;

namespace om {
namespace handler {

void get_seg_bbox(server::bbox& _return, const std::string&path, const int32_t segId)
{
    utility::timer t;
    if (!file::exists(path)) {
        throw common::argException("Cannot find path");
    }

    const uint32_t pageSize = 100000;
    const uint32_t pageNum = segId / pageSize;
    const uint32_t idx = segId % pageSize;
    const std::string fname = str( boost::format("%1%/segment_page%2%.data.ver4")
                                   % path % pageNum);

    datalayer::memMappedFile<segment::data> page(fname);

    const segment::data& d = page.GetPtr()[idx];

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
