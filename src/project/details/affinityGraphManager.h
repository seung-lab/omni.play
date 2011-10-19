#pragma once

#include "common/common.h"
#include "common/om.hpp"
#include "system/genericManager.hpp"
#include "volume/affinityGraph.h"

class affinityGraphManager{
public:
    affinityGraph& Get(const common::id id);
    affinityGraph& Add();
    void Remove(const common::id id);
    bool IsValid(const common::id id);
    const common::idSet& GetValidIds();
    bool IsEnabled(const common::id id);
    void SetEnabled(const common::id id, const bool enable);

private:
    genericManager<affinityGraph> graphs_;

    friend QDataStream&operator<<(QDataStream& out, const affinityGraphManager&);
    friend QDataStream&operator>>(QDataStream& in, affinityGraphManager&);
};

