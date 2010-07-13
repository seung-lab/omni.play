#include "datalayer/fs/omActionLoggerFS.h"

#include "segment/actions/segment/omSegmentSplitAction.h"
#include "segment/actions/segment/omSegmentGroupAction.h"
#include "segment/actions/segment/omSegmentJoinAction.h"
#include "segment/actions/segment/omSegmentSelectAction.h"
#include "segment/actions/segment/omSegmentValidateAction.h"

#include "segment/actions/voxel/omVoxelSelectionAction.h"
#include "segment/actions/voxel/omVoxelSelectionSetAction.h"
#include "segment/actions/voxel/omVoxelSetAction.h"
#include "segment/actions/voxel/omVoxelSetConnectedAction.h"
#include "segment/actions/voxel/omVoxelSetValueAction.h"

void OmActionLoggerFS::save(OmSegmentSplitAction* action, const std::string &)
{
}

void OmActionLoggerFS::save(OmSegmentGroupAction* action, const std::string &)
{
}

void OmActionLoggerFS::save(OmSegmentJoinAction* action, const std::string &)
{
}

void OmActionLoggerFS::save(OmSegmentSelectAction* action, const std::string &)
{
}

void OmActionLoggerFS::save(OmSegmentValidateAction* action, const std::string &)
{
}


void OmActionLoggerFS::save(OmVoxelSelectionAction* action, const std::string &)
{
}

void OmActionLoggerFS::save(OmEditSelectionApplyAction* action, const std::string &)
{
}

void OmActionLoggerFS::save(OmVoxelSetAction* action, const std::string &)
{
}

void OmActionLoggerFS::save(OmVoxelSetConnectedAction* action, const std::string &)
{
}

void OmActionLoggerFS::save(OmVoxelSetValueAction* action, const std::string &)
{
}

