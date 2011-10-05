#include "project/details/omAffinityGraphManager.h"

affinityGraph& affinityGraphManager::Get(const OmID id){
    return graphs_.Get(id);
}

affinityGraph& affinityGraphManager::Add(){
    return graphs_.Add();
}

void affinityGraphManager::Remove(const OmID id){
    graphs_.Remove(id);
}

bool affinityGraphManager::IsValid(const OmID id){
    return graphs_.IsValid(id);
}

const OmIDsSet& affinityGraphManager::GetValidIds(){
    return graphs_.GetValidIds();
}

bool affinityGraphManager::IsEnabled(const OmID id){
    return graphs_.IsEnabled(id);
}

void affinityGraphManager::SetEnabled(const OmID id, const bool enable){
    graphs_.SetEnabled(id, enable);
}
