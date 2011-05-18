#include "actions/omActions.h"
#include "common/omCommon.h"
#include "datalayer/fs/omFileNames.hpp"
#include "project/details/omChannelManager.h"
#include "project/details/omProjectVolumes.h"
#include "project/details/omSegmentationManager.h"
#include "utility/dataWrappers.h"
#include "volume/omFilter2d.h"

OmSegmentation& OmSegmentationManager::GetSegmentation(const OmID id)
{
    return manager_.Get(id);
}

OmSegmentation& OmSegmentationManager::AddSegmentation()
{
    OmSegmentation& vol = manager_.Add();
    OmFileNames::MakeVolFolder(&vol);
    OmActions::Save();
    return vol;
}

void OmSegmentationManager::RemoveSegmentation(const OmID id)
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
                OmSegmentation* segmentation = filter->GetSegmentation();

                if(segmentation->getID() == id){
                    filter->SetSegmentation(0);
                }
            }
        }
    }

    GetSegmentation(id).CloseDownThreads();

    //TODO: fixme
    //OmDataPath path(GetSegmentation(id).GetDirectoryPath());
    //OmProjectData::DeleteInternalData(path);

    manager_.Remove(id);

    OmActions::Save();
}

bool OmSegmentationManager::IsSegmentationValid(const OmID id){
    return manager_.IsValid(id);
}

const OmIDsSet& OmSegmentationManager::GetValidSegmentationIds(){
    return manager_.GetValidIds();
}

bool OmSegmentationManager::IsSegmentationEnabled(const OmID id){
    return manager_.IsEnabled(id);
}

void OmSegmentationManager::SetSegmentationEnabled(const OmID id, const bool enable){
    manager_.SetEnabled(id, enable);
}

const std::vector<OmSegmentation*> OmSegmentationManager::GetPtrVec() const{
    return manager_.GetPtrVec();
}
