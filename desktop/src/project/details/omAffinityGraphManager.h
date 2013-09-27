#pragma once

#include "common/common.h"
#include "system/omGenericManager.hpp"
#include "volume/omAffinityGraph.h"

class OmAffinityGraphManager{
public:
    OmAffinityGraph& Get(const om::common::ID id);
    OmAffinityGraph& Add();
    void Remove(const om::common::ID id);
    bool IsValid(const om::common::ID id);
    const om::common::IDSet& GetValidIds();
    bool IsEnabled(const om::common::ID id);
    void SetEnabled(const om::common::ID id, const bool enable);

private:
    OmGenericManager<OmAffinityGraph> graphs_;

    friend QDataStream& operator<<(QDataStream& out, const OmAffinityGraphManager&);
    friend QDataStream& operator>>(QDataStream& in, OmAffinityGraphManager&);
    friend YAML::Emitter& YAML::operator<<(YAML::Emitter& out, const OmAffinityGraphManager&);
    friend void YAML::operator>>(const YAML::Node& in, OmAffinityGraphManager&);
};

