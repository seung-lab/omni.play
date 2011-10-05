#include "project/details/omAffinityGraphManager.h"
#include "project/details/channelManager.h"
#include "project/details/projectVolumes.h"
#include "project/details/segmentationManager.h"

projectVolumes::projectVolumes()
    : channels_(new channelManager())
    , segmentations_(new segmentationManager(this))
    , affGraphs_(new affinityGraphManager())
{}

projectVolumes::~projectVolumes()
{}

