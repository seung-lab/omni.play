#include "actions/omActions.h"
#include "common/common.h"
#include "project/details/omChannelManager.h"
#include "project/details/omProjectVolumes.h"
#include "project/details/omSegmentationManager.h"
#include "users/omUsers.h"
#include "utility/channelDataWrapper.hpp"
#include "volume/metadataDataSource.hpp"
#include "volume/omFilter2d.h"
#include "volume/omSegmentation.h"

#include <boost/filesystem.hpp>

template class om::common::GenericManager<OmSegmentation>;

om::file::path OmSegmentationManager::path() {
  return OmProject::Paths().Segmentations() / "segmentation";
}

OmSegmentation* OmSegmentationManager::GetSegmentation(
    const om::common::ID id) {
  try {
    return &manager_.Get(id);
  }
  catch (...) {
    return nullptr;
  }
}

OmSegmentation& OmSegmentationManager::AddSegmentation() {
  return manager_.Add(path());
}

void OmSegmentationManager::Load() {
  for (auto& folder : om::fs::VolumeFolders::FindSegmentations()) {
    om::file::path p = path();
    p += std::to_string(folder.id);
    if (om::volume::MetadataDataSource::GetStatic(p.string())) {
      log_infos(io) << "Loading Segmentation " << folder.id;
      manager_.Insert(folder.id, new OmSegmentation(folder.id, path()));
    }
  }
}

void OmSegmentationManager::RemoveSegmentation(const om::common::ID id) {
  auto seg = GetSegmentation(id);
  if (seg) {
    for (auto channelID : volumes_.Channels().GetValidChannelIds()) {
      ChannelDataWrapper cdw(channelID);

      for (auto* filter : cdw.GetFilters()) {
        if (om::OVERLAY_SEGMENTATION == filter->FilterType()) {
          OmSegmentation* segmentation = filter->GetSegmentation();

          if (segmentation->id() == id) {
            filter->SetSegmentation(0);
          }
        }
      }
    }

    seg->CloseDownThreads();

    // TODO: fixme
    // OmDataPath path(GetSegmentation(id).GetDirectoryPath());
    // OmProjectData::DeleteInternalData(path);

    manager_.Remove(id);

    OmActions::Save();
  }
}

bool OmSegmentationManager::IsSegmentationValid(const om::common::ID id) {
  return manager_.IsValid(id);
}

const om::common::IDSet& OmSegmentationManager::GetValidSegmentationIds() {
  return manager_.GetValidIds();
}

bool OmSegmentationManager::IsSegmentationEnabled(const om::common::ID id) {
  return manager_.IsEnabled(id);
}

void OmSegmentationManager::SetSegmentationEnabled(const om::common::ID id,
                                                   const bool enable) {
  manager_.SetEnabled(id, enable);
}

const std::vector<OmSegmentation*> OmSegmentationManager::GetPtrVec() const {
  return manager_.GetPtrVec();
}
