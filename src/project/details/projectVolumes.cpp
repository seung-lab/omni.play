#include "project/details/affinityGraphManager.h"
#include "project/details/channelManager.h"
#include "project/details/projectVolumes.h"
#include "project/details/segmentationManager.h"

namespace om {
namespace proj {

volumes::volumes()
    : channels_(new channelManager())
    , segmentations_(new segmentationManager(this))
    , affGraphs_(new affinityGraphManager())
{}

volumes::~volumes()
{}

}
}
