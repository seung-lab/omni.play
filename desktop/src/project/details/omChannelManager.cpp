#include "actions/omActions.h"
#include "common/common.h"
#include "datalayer/fs/omFileNames.hpp"
#include "project/details/omChannelManager.h"
#include "volume/omChannelFolder.h"
#include "volume/omFilter2d.h"

OmChannel& OmChannelManager::GetChannel(const OmID id)
{
    return manager_.Get(id);
}

OmChannel& OmChannelManager::AddChannel()
{
    OmChannel& vol = manager_.Add();
    vol.Folder()->MakeVolFolder();
    OmActions::Save();
    return vol;
}

void OmChannelManager::RemoveChannel(const OmID id)
{
    GetChannel(id).CloseDownThreads();

    //TODO: fixme
    //OmDataPath path(GetChannel(id).GetDirectoryPath());
    //OmProjectData::DeleteInternalData(path);

    manager_.Remove(id);

    OmActions::Save();
}

bool OmChannelManager::IsChannelValid(const OmID id){
    return manager_.IsValid(id);
}

const OmIDsSet& OmChannelManager::GetValidChannelIds(){
    return manager_.GetValidIds();
}

bool OmChannelManager::IsChannelEnabled(const OmID id){
    return manager_.IsEnabled(id);
}

void OmChannelManager::SetChannelEnabled(const OmID id, bool enable){
    manager_.SetEnabled(id, enable);
}

const std::vector<OmChannel*> OmChannelManager::GetPtrVec() const {
    return manager_.GetPtrVec();
}
