#pragma once

#include "common/common.h"
#include "common/om.hpp"
#include "system/genericManager.hpp"
#include "volume/affinityGraph.h"

class affinityGraphManager{
public:
    affinityGraph& Get(const OmID id);
    affinityGraph& Add();
    void Remove(const OmID id);
    bool IsValid(const OmID id);
    const OmIDsSet& GetValidIds();
    bool IsEnabled(const OmID id);
    void SetEnabled(const OmID id, const bool enable);

private:
    genericManager<affinityGraph> graphs_;

    friend QDataStream&operator<<(QDataStream& out, const affinityGraphManager&);
    friend QDataStream&operator>>(QDataStream& in, affinityGraphManager&);
};

