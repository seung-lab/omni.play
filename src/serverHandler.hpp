#include "thrift/server.h"
#include "common/common.h"
#include "project/project.h"
#include "project/details/projectVolumes.h"
#include "project/details/channelManager.h"
#include "project/details/segmentationManager.h"
#include "volume/channel.h"
#include "volume/segmentation.h"

using namespace om::common;

namespace om {
namespace server {

class serverHandler : virtual public serverIf
{
private:
    volume::channel* chan;
    volume::segmentation* seg;

public:
    serverHandler()
    {
        project::project::Load("/Users/balkamm/omniData/FirstValidatedVolume.omni");
        seg = &project::project::Volumes().Segmentations().GetSegmentation(1);
        chan = &project::project::Volumes().Channels().GetChannel(1);
    }

    void get_chan_tile(tile& _return, const vector3d& point)
    {
        coords::globalCoord global(point.x, point.y, point.z);
        coords::chunkCoord cc = global.toChunkCoord(*chan, 0);
        boost::shared_ptr<chunks::chunk> chunk = chan->GetChunk(cc);
    }

    void get_seg_tile(tile& _return, const vector3d& point)
    {
        // Your implementation goes here
        printf("get_seg_tile\n");
    }

    void click(click_info& _return, const vector3d& point)
    {
        // Your implementation goes here
        printf("click\n");
    }

    double compare_results(const std::vector<result> & old_results, const result & new_result)
    {
        // Your implementation goes here
        printf("compare_results\n");
        return 0;
    }

};

} // namespace server
} // namespace om
