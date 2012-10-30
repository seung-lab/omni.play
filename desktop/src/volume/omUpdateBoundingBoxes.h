#pragma once

#include "common/common.h"

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
    void doUpdate(const om::coords::Chunk& coord);
};

