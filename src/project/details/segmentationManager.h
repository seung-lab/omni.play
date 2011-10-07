#pragma once

#include "common/common.h"
#include "system/genericManager.hpp"
#include "volume/segmentation.h"
#include "datalayer/archive/segmentation.h"

class projectVolumes;

class segmentationManager {
private:
    projectVolumes *const volumes_;

public:
    segmentationManager(projectVolumes* volumes)
        : volumes_(volumes)
    {}

    segmentation& GetSegmentation(const common::id id);
    segmentation& AddSegmentation();
    void RemoveSegmentation(const common::id id);
    bool IsSegmentationValid(const common::id id);
    const common::idsSet & GetValidSegmentationIds();
    bool IsSegmentationEnabled(const common::id id);
    void SetSegmentationEnabled(const common::id id, const bool enable);
    const std::vector<segmentation*> GetPtrVec() const;

private:
    genericManager<segmentation> manager_;

    friend YAML::Emitter& YAML::operator<<(YAML::Emitter& out, const segmentationManager&);
    friend void YAML::operator>>(const YAML::Node& in, segmentationManager&);
    friend QDataStream& operator<<(QDataStream& out, const segmentationManager&);
    friend QDataStream& operator>>(QDataStream& in, segmentationManager&);
};

