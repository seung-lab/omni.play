#include "actions/omActions.h"
#include "omFilter2dManager.h"
#include "omFilter2d.h"

OmFilter2dManager::OmFilter2dManager()
{}

OmFilter2d& OmFilter2dManager::AddFilter()
{
    OmFilter2d &r_filter = filters_.Add();
    OmActions::Save();
    return r_filter;
}

OmFilter2d& OmFilter2dManager::GetFilter(OmID id){
    return filters_.Get(id);
}

const OmIDsSet & OmFilter2dManager::GetValidFilterIds(){
    return filters_.GetValidIds();
}

bool OmFilter2dManager::IsFilterEnabled(OmID id){
    return filters_.IsEnabled(id);
}

bool OmFilter2dManager::IsFilterValid(OmID id){
    return filters_.IsValid(id);
}

std::vector<OmFilter2d*> OmFilter2dManager::GetFilters()
{
    std::vector<OmFilter2d*> filters;

    FOR_EACH(iter, GetValidFilterIds()){
        filters.push_back(&filters_.Get(*iter));
    }

    return filters;
}

