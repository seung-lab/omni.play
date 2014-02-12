#include <boost/filesystem.hpp>
#include "actions/omActions.h"
#include "common/common.h"
#include "datalayer/file.h"
#include "datalayer/fs/volumeFolders.hpp"
#include "project/details/omChannelManager.h"
#include "volume/metadataDataSource.hpp"
#include "volume/omFilter2d.h"

template class om::common::GenericManager<OmChannel>;

OmChannel* OmChannelManager::GetChannel(const om::common::ID id) {
  try {
    return &manager_.Get(id);
  }
  catch (...) {
    return nullptr;
  }
}

OmChannel& OmChannelManager::AddChannel() {
  return manager_.Add(OmProject::Paths().Channels() / "channel");
}

void OmChannelManager::Load() {
  for (auto& folder : om::fs::VolumeFolders::FindChannels()) {
    om::file::path p = OmProject::Paths().Channels() / "channel";
    p += std::to_string(folder.id);
    if (om::volume::MetadataDataSource::GetStatic(p.string())) {
      log_infos(io) << "Loading Channel " << folder.id;
      manager_.Insert(
          folder.id,
          new OmChannel(folder.id, OmProject::Paths().Channels() / "channel"));
    }
  }
}

void OmChannelManager::RemoveChannel(const om::common::ID id) {
  auto c = GetChannel(id);
  if (c) {
    c->CloseDownThreads();

    manager_.Remove(id);
    om::file::RemoveDir(OmProject::Paths().Channel(id));

    OmActions::Save();
  } else {
    log_debugs(unknown) << "Unable to remove channel " << id << ".  Not Found.";
  }
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
