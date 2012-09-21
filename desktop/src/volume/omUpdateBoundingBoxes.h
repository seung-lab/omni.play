#pragma once

#include "common/omCommon.h"

class OmSegmentation;
class OmSegments;

class OmUpdateBoundingBoxes {
private:
    OmSegmentation *const vol_;
    OmSegments *const segments_;

public:
    OmUpdateBoundingBoxes(OmSegmentation* vol);
    void Update();

private:
    void doUpdate(const om::chunkCoord& coord);
};

