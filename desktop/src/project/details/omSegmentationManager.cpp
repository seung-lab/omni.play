#include "actions/omActions.h"
#include "common/common.h"

#include "project/details/omChannelManager.h"
#include "project/details/omProjectVolumes.h"
#include "project/details/omSegmentationManager.h"
#include "utility/dataWrappers.h"
#include "volume/omFilter2d.h"

OmSegmentation* OmSegmentationManager::GetSegmentation(
    const om::common::ID id) {
  return manager_.Get(id);
}

OmSegmentation& OmSegmentationManager::AddSegmentation() {
  OmSegmentation& vol = manager_.Add();
  om::file::MkDir(vol.SegPaths());
  OmActions::Save();
  return vol;
}

void OmSegmentationManager::RemoveSegmentation(const om::common::ID id) {
  auto seg = GetSegmentation(id);
  if (!seg) {
    return;
  }

  FOR_EACH(channelID, volumes_->Channels().GetValidChannelIds()) {
    ChannelDataWrapper cdw(*channelID);

    const std::vector<OmFilter2d*> filters = cdw.GetFilters();

    FOR_EACH(iter, filters) {
      OmFilter2d* filter = *iter;

      if (om::OVERLAY_SEGMENTATION == filter->FilterType()) {
        OmSegmentation* segmentation = filter->GetSegmentation();

        if (segmentation->GetID() == id) {
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
