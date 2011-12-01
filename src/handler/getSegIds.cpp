#include "handler/handler.h"
#include "handler/validate.hpp"

#include "pipeline/mapData.hpp"
#include "pipeline/getSegIds.hpp"

#include "utility/timer.hpp"

using namespace std;
using namespace boost;

namespace om {
namespace handler {

using namespace pipeline;

int32_t get_seg_id(const server::metadata& vol, const server::vector3d& point)
{
    utility::timer t;
    validateMetadata(vol);

    coords::volumeSystem coordSystem(vol);

    coords::globalCoord coord = point;
    coords::dataCoord dc = coord.toDataCoord(&coordSystem, vol.mipLevel);

    mapData dataSrc(vol.uri, vol.type);

    data_var id = dataSrc >> getSegIds(dc);

    cout << "get_seg_id done: " << t.s_elapsed() << " seconds" << endl;

    return get<data<uint32_t> >(id).data.get()[0];
}

void get_seg_ids(vector<int32_t> & _return,
                 const server::metadata& vol,
                 const server::vector3d& point,
                 const double radius,
                 const server::viewType::type view)
{
    utility::timer t;
    validateMetadata(vol);

    coords::volumeSystem coordSystem(vol);

    coords::globalCoord coord = point;
    coords::dataCoord dc = coord.toDataCoord(&coordSystem, vol.mipLevel);

    mapData dataSrc(vol.uri, vol.type);

    data_var ids = dataSrc >> getSegIds(dc, radius, view);

    data<uint32_t> out = get<data<uint32_t> >(ids);

    _return.resize(out.size);
    uint32_t* outData = out.data.get();
    copy(outData, &outData[out.size], _return.begin());
    cout << "get_seg_id done: " << t.s_elapsed() << " seconds" << endl;
}

}
}
