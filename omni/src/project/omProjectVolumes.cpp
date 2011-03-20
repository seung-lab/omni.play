#include "project/omSegmentationManager.h"
#include "project/omProjectVolumes.h"
#include "project/omAffinityGraphManager.h"
#include "project/omChannelManager.h"

OmProjectVolumes::OmProjectVolumes()
	: channels_(new OmChannelManager())
	, segmentations_(new OmSegmentationManager(this))
	, affGraphs_(new OmAffinityGraphManager())
{}

OmProjectVolumes::~OmProjectVolumes()
{
	delete affGraphs_; // graphs vould depend on channels...
	delete channels_;
	delete segmentations_;
}
