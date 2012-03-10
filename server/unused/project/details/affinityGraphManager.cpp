#include "project/details/omAffinityGraphManager.h"

affinityGraph& affinityGraphManager::Get(const common::id id){
    return graphs_.Get(id);
}

affinityGraph& affinityGraphManager::Add(){
    return graphs_.Add();
}

void affinityGraphManager::Remove(const common::id id){
    graphs_.Remove(id);
}

bool affinityGraphManager::IsValid(const common::id id){
    return graphs_.IsValid(id);
}

const common::idSet& affinityGraphManager::GetValidIds(){
    return graphs_.GetValidIds();
}

bool affinityGraphManager::IsEnabled(const common::id id){
    return graphs_.IsEnabled(id);
}

void affinityGraphManager::SetEnabled(const common::id id, const bool enable){
    graphs_.SetEnabled(id, enable);
}
