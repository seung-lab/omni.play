#pragma once

#include "common/common.h"
#include "common/genericManager.hpp"
#include "volume/segmentation.h"
#include "datalayer/archive/segmentation.h"

namespace om {
namespace proj {

class volumes;

class segmentationManager {
private:
    volumes *const volumes_;

public:
    segmentationManager(volumes* vols)
        : volumes_(vols)
    {}

    volume::segmentation& GetSegmentation(const common::id id);
    volume::segmentation& AddSegmentation();
    void RemoveSegmentation(const common::id id);
    bool IsSegmentationValid(const common::id id);
    const common::idSet & GetValidSegmentationIds();
    bool IsSegmentationEnabled(const common::id id);
    void SetSegmentationEnabled(const common::id id, const bool enable);
    const std::vector<volume::segmentation*> GetPtrVec() const;

private:
    common::genericManager<volume::segmentation> manager_;

    friend YAML::Emitter& YAML::operator<<(YAML::Emitter& out, const segmentationManager&);
    friend void YAML::operator>>(const YAML::Node& in, segmentationManager&);
};

}
}
