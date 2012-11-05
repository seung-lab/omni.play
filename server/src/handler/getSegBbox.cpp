#include "handler/handler.h"
#include "handler/validate.hpp"

#include "common/common.h"
#include "datalayer/memMappedFile.hpp"
#include "utility/timer.hpp"
#include "utility/convert.hpp"
#include "segment/segmentTypes.hpp"
#include "volume/volume.h"

#include "boost/format.hpp"

using namespace std;
using namespace boost;

namespace om {
namespace handler {

void get_seg_data(server::segData& _return, const volume::volume& vol, const int32_t segId)
{
    utility::timer t;
    const segments::data& d = vol.GetSegmentData(segId);
    coords::DataBbox dc(d.bounds, &vol.CoordSystem(), 0);

    _return.bounds = utility::Convert(dc.ToGlobalBbox());
    _return.size = d.size;
}
void get_seg_list_data(std::map<int32_t, server::segData>& _return,
                       const volume::volume& vol,
                       const std::set<int32_t>& segIds)
{
	FOR_EACH(id, segIds)
	{
		get_seg_data(_return[*id], vol, *id);
	}
}


}
}
