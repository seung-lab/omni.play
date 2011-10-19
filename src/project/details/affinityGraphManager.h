#pragma once

#include "common/common.h"
#include "common/genericManager.hpp"
#include "volume/affinityGraph.h"

namespace om {
namespace proj {

class affinityGraphManager{
public:
    volume::affinityGraph& Get(const common::id id);
    volume::affinityGraph& Add();
    void Remove(const common::id id);
    bool IsValid(const common::id id);
    const common::idSet& GetValidIds();
    bool IsEnabled(const common::id id);
    void SetEnabled(const common::id id, const bool enable);

private:
    common::genericManager<volume::affinityGraph> graphs_;
};

}
}
