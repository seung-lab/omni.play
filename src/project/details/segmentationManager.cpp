#include "actions/omActions.h"
#include "common/common.h"
#include "datalayer/fs/fileNames.hpp"
#include "project/details/channelManager.h"
#include "project/details/projectVolumes.h"
#include "project/details/segmentationManager.h"
#include "utility/dataWrappers.h"
#include "volume/omFilter2d.h"
#include "volume/segmentationFolder.h"

segmentation& segmentationManager::GetSegmentation(const common::id id)
{
    return manager_.Get(id);
}

segmentation& segmentationManager::AddSegmentation()
{
    segmentation& vol = manager_.Add();
    vol.Folder()->MakeVolFolder();
    OmActions::Save();
    return vol;
}

void segmentationManager::RemoveSegmentation(const common::id id)
{
    FOR_EACH(channelID, volumes_->Channels().GetValidChannelIds())
    {
        ChannelDataWrapper cdw(*channelID);

        const std::vector<OmFilter2d*> filters = cdw.GetFilters();

        FOR_EACH(iter, filters)
        {
            OmFilter2d* filter = *iter;

            if(om::OVERLAY_SEGMENTATION == filter->FilterType())
            {
                segmentation* segmentation = filter->GetSegmentation();

                if(segmentation->getID() == id){
                    filter->SetSegmentation(0);
                }
            }
        }
    }

    GetSegmentation(id).CloseDownThreads();

    //TODO: fixme
    //dataPath path(GetSegmentation(id).GetDirectoryPath());
    //projectData::DeleteInternalData(path);

    manager_.Remove(id);

    OmActions::Save();
}

bool segmentationManager::IsSegmentationValid(const common::id id){
    return manager_.IsValid(id);
}

const common::idsSet& segmentationManager::GetValidSegmentationIds(){
    return manager_.GetValidIds();
}

bool segmentationManager::IsSegmentationEnabled(const common::id id){
    return manager_.IsEnabled(id);
}

void segmentationManager::SetSegmentationEnabled(const common::id id, const bool enable){
    manager_.SetEnabled(id, enable);
}

const std::vector<segmentation*> segmentationManager::GetPtrVec() const{
    return manager_.GetPtrVec();
}
