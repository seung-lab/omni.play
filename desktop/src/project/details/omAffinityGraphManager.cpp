#include "project/details/omAffinityGraphManager.h"

OmAffinityGraph* OmAffinityGraphManager::Get(const om::common::ID id) {
  return graphs_.Get(id);
}

OmAffinityGraph& OmAffinityGraphManager::Add() { return graphs_.Add(); }

void OmAffinityGraphManager::Remove(const om::common::ID id) {
  graphs_.Remove(id);
}

bool OmAffinityGraphManager::IsValid(const om::common::ID id) {
  return graphs_.IsValid(id);
}

const om::common::IDSet& OmAffinityGraphManager::GetValidIds() {
  return graphs_.GetValidIds();
}

bool OmAffinityGraphManager::IsEnabled(const om::common::ID id) {
  return graphs_.IsEnabled(id);
}

void OmAffinityGraphManager::SetEnabled(const om::common::ID id,
                                        const bool enable) {
  graphs_.SetEnabled(id, enable);
}
