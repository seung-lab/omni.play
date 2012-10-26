#pragma once

#include "common/common.h"
#include "system/omGenericManager.hpp"
#include "volume/omSegmentation.h"
#include "datalayer/archive/segmentation.h"

class OmProjectVolumes;

class OmSegmentationManager {
private:
    OmProjectVolumes *const volumes_;

public:
    OmSegmentationManager(OmProjectVolumes* volumes)
        : volumes_(volumes)
    {}

    OmSegmentation& GetSegmentation(const OmID id);
    OmSegmentation& AddSegmentation();
    void RemoveSegmentation(const OmID id);
    bool IsSegmentationValid(const OmID id);
    const OmIDsSet & GetValidSegmentationIds();
    bool IsSegmentationEnabled(const OmID id);
    void SetSegmentationEnabled(const OmID id, const bool enable);
    const std::vector<OmSegmentation*> GetPtrVec() const;

private:
    OmGenericManager<OmSegmentation> manager_;

    friend YAML::Emitter& YAML::operator<<(YAML::Emitter& out, const OmSegmentationManager&);
    friend void YAML::operator>>(const YAML::Node& in, OmSegmentationManager&);
    friend QDataStream& operator<<(QDataStream& out, const OmSegmentationManager&);
    friend QDataStream& operator>>(QDataStream& in, OmSegmentationManager&);
};

