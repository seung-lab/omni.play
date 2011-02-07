#include "actions/omActions.h"
#include "common/omCommon.h"
#include "project/omChannelManager.h"
#include "project/omProjectVolumes.h"
#include "project/omSegmentationManager.h"
#include "volume/omFilter2d.h"

OmSegmentation& OmSegmentationManager::GetSegmentation(const OmID id)
{
    return mSegmentationManager.Get(id);
}

OmSegmentation& OmSegmentationManager::AddSegmentation()
{
    OmSegmentation& r_segmentation = mSegmentationManager.Add();
    OmActions::Save();
    return r_segmentation;
}

void OmSegmentationManager::RemoveSegmentation(const OmID id)
{
    FOR_EACH(channelID, volumes_->Channels().GetValidChannelIds()){

        ChannelDataWrapper cdw(*channelID);

        const std::vector<OmFilter2d*> filters = cdw.GetFilters();

        FOR_EACH(iter, filters){
            OmFilter2d* filter = *iter;

            if(filter->GetSegmentationWrapper().GetSegmentationID() == id){
                filter->SetSegmentation(0);
            }
        }
    }

    GetSegmentation(id).CloseDownThreads();

    //TODO: fixme
    //OmDataPath path(GetSegmentation(id).GetDirectoryPath());
    //OmProjectData::DeleteInternalData(path);

    mSegmentationManager.Remove(id);

    OmActions::Save();
}

bool OmSegmentationManager::IsSegmentationValid(const OmID id)
{
    return mSegmentationManager.IsValid(id);
}

const OmIDsSet& OmSegmentationManager::GetValidSegmentationIds()
{
    return mSegmentationManager.GetValidIds();
}

bool OmSegmentationManager::IsSegmentationEnabled(const OmID id)
{
    return mSegmentationManager.IsEnabled(id);
}

void OmSegmentationManager::SetSegmentationEnabled(const OmID id, const bool enable)
{
    mSegmentationManager.SetEnabled(id, enable);
}
