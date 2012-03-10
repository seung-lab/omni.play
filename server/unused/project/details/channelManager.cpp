#include "common/common.h"
#include "datalayer/fs/fileNames.hpp"
#include "project/details/channelManager.h"
#include "volume/channelFolder.h"

namespace om {
namespace proj {

volume::channel& channelManager::GetChannel(const common::id id)
{
    return manager_.Get(id);
}

volume::channel& channelManager::AddChannel()
{
    volume::channel& vol = manager_.Add();
    vol.Folder()->MakeVolFolder();
    return vol;
}

void channelManager::RemoveChannel(const common::id id) {
    manager_.Remove(id);
}

bool channelManager::IsChannelValid(const common::id id){
    return manager_.IsValid(id);
}

const common::idSet& channelManager::GetValidChannelIds(){
    return manager_.GetValidIds();
}

bool channelManager::IsChannelEnabled(const common::id id){
    return manager_.IsEnabled(id);
}

void channelManager::SetChannelEnabled(const common::id id, bool enable){
    manager_.SetEnabled(id, enable);
}

const std::vector<volume::channel*> channelManager::GetPtrVec() const {
    return manager_.GetPtrVec();
}

} // namespace proj
} // namespace om
