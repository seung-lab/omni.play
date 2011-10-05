#pragma once

#include "common/omCommon.h"
#include "system/omGenericManager.hpp"
#include "volume/segmentation.h"
#include "datalayer/archive/segmentation.h"

class OmProjectVolumes;

class segmentationManager {
private:
    OmProjectVolumes *const volumes_;

public:
    segmentationManager(OmProjectVolumes* volumes)
        : volumes_(volumes)
    {}

    segmentation& GetSegmentation(const OmID id);
    segmentation& AddSegmentation();
    void RemoveSegmentation(const OmID id);
    bool IsSegmentationValid(const OmID id);
    const OmIDsSet & GetValidSegmentationIds();
    bool IsSegmentationEnabled(const OmID id);
    void SetSegmentationEnabled(const OmID id, const bool enable);
    const std::vector<segmentation*> GetPtrVec() const;

private:
    OmGenericManager<segmentation> manager_;

    friend YAML::Emitter& YAML::operator<<(YAML::Emitter& out, const segmentationManager&);
    friend void YAML::operator>>(const YAML::Node& in, segmentationManager&);
    friend QDataStream& operator<<(QDataStream& out, const segmentationManager&);
    friend QDataStream& operator>>(QDataStream& in, segmentationManager&);
};

