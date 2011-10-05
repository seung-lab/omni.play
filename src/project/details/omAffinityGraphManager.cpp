#include "project/details/omAffinityGraphManager.h"

OmAffinityGraph& OmAffinityGraphManager::Get(const OmID id){
    return graphs_.Get(id);
}

OmAffinityGraph& OmAffinityGraphManager::Add(){
    return graphs_.Add();
}

void OmAffinityGraphManager::Remove(const OmID id){
    graphs_.Remove(id);
}

bool OmAffinityGraphManager::IsValid(const OmID id){
    return graphs_.IsValid(id);
}

const OmIDsSet& OmAffinityGraphManager::GetValidIds(){
    return graphs_.GetValidIds();
}

bool OmAffinityGraphManager::IsEnabled(const OmID id){
    return graphs_.IsEnabled(id);
}

void OmAffinityGraphManager::SetEnabled(const OmID id, const bool enable){
    graphs_.SetEnabled(id, enable);
}
