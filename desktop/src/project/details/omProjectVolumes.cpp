#include "project/details/omAffinityGraphManager.h"
#include "project/details/omChannelManager.h"
#include "project/details/omProjectVolumes.h"
#include "project/details/omSegmentationManager.h"

OmProjectVolumes::OmProjectVolumes()
    : channels_(new OmChannelManager()),
      segmentations_(new OmSegmentationManager(this)),
      affGraphs_(new OmAffinityGraphManager()) {}

OmProjectVolumes::~OmProjectVolumes() {}
