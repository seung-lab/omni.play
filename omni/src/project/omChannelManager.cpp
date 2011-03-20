#include "project/omChannelManager.h"
#include "actions/omActions.h"
#include "common/omCommon.h"
#include "volume/omFilter2d.h"

OmChannel& OmChannelManager::GetChannel(const OmID id)
{
	return mChannelManager.Get(id);
}

OmChannel& OmChannelManager::AddChannel()
{
	OmChannel& r_channel = mChannelManager.Add();
	OmActions::Save();
	return r_channel;
}

void OmChannelManager::RemoveChannel(const OmID id)
{
	GetChannel(id).CloseDownThreads();

	//TODO: fixme
	//OmDataPath path(GetChannel(id).GetDirectoryPath());
	//OmProjectData::DeleteInternalData(path);

	mChannelManager.Remove(id);

	OmActions::Save();
}

bool OmChannelManager::IsChannelValid(const OmID id)
{
	return mChannelManager.IsValid(id);
}

const OmIDsSet& OmChannelManager::GetValidChannelIds()
{
	return mChannelManager.GetValidIds();
}

bool OmChannelManager::IsChannelEnabled(const OmID id)
{
	return mChannelManager.IsEnabled(id);
}

void OmChannelManager::SetChannelEnabled(const OmID id, bool enable)
{
	mChannelManager.SetEnabled(id, enable);
}
