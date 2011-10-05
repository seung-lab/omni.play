#include "actions/omActions.h"
#include "common/common.h"
#include "datalayer/fs/omFileNames.hpp"
#include "project/details/channelManager.h"
#include "volume/channelFolder.h"
#include "volume/omFilter2d.h"

channel& channelManager::GetChannel(const OmID id)
{
    return manager_.Get(id);
}

channel& channelManager::AddChannel()
{
    channel& vol = manager_.Add();
    vol.Folder()->MakeVolFolder();
    OmActions::Save();
    return vol;
}

void channelManager::RemoveChannel(const OmID id)
{
    GetChannel(id).CloseDownThreads();

    //TODO: fixme
    //OmDataPath path(GetChannel(id).GetDirectoryPath());
    //projectData::DeleteInternalData(path);

    manager_.Remove(id);

    OmActions::Save();
}

bool channelManager::IsChannelValid(const OmID id){
    return manager_.IsValid(id);
}

const OmIDsSet& channelManager::GetValidChannelIds(){
    return manager_.GetValidIds();
}

bool channelManager::IsChannelEnabled(const OmID id){
    return manager_.IsEnabled(id);
}

void channelManager::SetChannelEnabled(const OmID id, bool enable){
    manager_.SetEnabled(id, enable);
}

const std::vector<channel*> channelManager::GetPtrVec() const {
    return manager_.GetPtrVec();
}
