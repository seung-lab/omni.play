#include "project/details/omAffinityGraphManager.h"
#include "project/details/channelManager.h"
#include "project/details/omProjectVolumes.h"
#include "project/details/segmentationManager.h"

OmProjectVolumes::OmProjectVolumes()
    : channels_(new channelManager())
    , segmentations_(new segmentationManager(this))
    , affGraphs_(new affinityGraphManager())
{}

OmProjectVolumes::~OmProjectVolumes()
{}

