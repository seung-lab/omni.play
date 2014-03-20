#include "actions/omActions.h"
#include "common/common.h"
#include "datalayer/fs/omFileNames.hpp"
#include "project/details/omChannelManager.h"
#include "volume/omChannelFolder.h"
#include "volume/omFilter2d.h"

OmChannel* OmChannelManager::GetChannel(const om::common::ID id) {
  return manager_.Get(id);
}

OmChannel& OmChannelManager::AddChannel() {
  OmChannel& vol = manager_.Add();
  vol.Folder().MakeVolFolder();
  OmActions::Save();
  return vol;
}

void OmChannelManager::RemoveChannel(const om::common::ID id) {
  auto chan = GetChannel(id);
  if (!chan) {
    return;
  }

  chan->CloseDownThreads();

  // TODO: fixme
  // OmDataPath path(GetChannel(id).GetDirectoryPath());
  // OmProjectData::DeleteInternalData(path);

  manager_.Remove(id);

  OmActions::Save();
}

bool OmChannelManager::IsChannelValid(const om::common::ID id) {
  return manager_.IsValid(id);
}

const om::common::IDSet& OmChannelManager::GetValidChannelIds() {
  return manager_.GetValidIds();
}

bool OmChannelManager::IsChannelEnabled(const om::common::ID id) {
  return manager_.IsEnabled(id);
}

void OmChannelManager::SetChannelEnabled(const om::common::ID id, bool enable) {
  manager_.SetEnabled(id, enable);
}

const std::vector<OmChannel*> OmChannelManager::GetPtrVec() const {
  return manager_.GetPtrVec();
}
