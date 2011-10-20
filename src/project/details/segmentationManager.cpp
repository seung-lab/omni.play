#include "common/common.h"
#include "datalayer/fs/fileNames.hpp"
#include "project/details/channelManager.h"
#include "project/details/projectVolumes.h"
#include "project/details/segmentationManager.h"
#include "volume/segmentationFolder.h"

namespace om {
namespace proj {

volume::segmentation& segmentationManager::GetSegmentation(const common::id id)
{
    return manager_.Get(id);
}

volume::segmentation& segmentationManager::AddSegmentation()
{
    volume::segmentation& vol = manager_.Add();
    vol.Folder()->MakeVolFolder();
    return vol;
}

void segmentationManager::RemoveSegmentation(const common::id id)
{
    manager_.Remove(id);
}

bool segmentationManager::IsSegmentationValid(const common::id id){
    return manager_.IsValid(id);
}

const common::idSet& segmentationManager::GetValidSegmentationIds(){
    return manager_.GetValidIds();
}

bool segmentationManager::IsSegmentationEnabled(const common::id id){
    return manager_.IsEnabled(id);
}

void segmentationManager::SetSegmentationEnabled(const common::id id, const bool enable){
    manager_.SetEnabled(id, enable);
}

const std::vector<volume::segmentation*> segmentationManager::GetPtrVec() const{
    return manager_.GetPtrVec();
}

} // namespace proj
} // namespace om
