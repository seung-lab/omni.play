#pragma once

#include "common/common.h"

struct MipLevelInfo {
    uint64_t factor;
    Vector3<uint64_t> volDims;

    uint64_t maxAllowedIndex;
    uint64_t volSlabSize;
    uint64_t volSliceSize;
    uint64_t volRowSize;
    uint64_t totalVoxels;
};

struct MippingInfo {
    int maxMipLevel;
    Vector3<uint64_t> chunkDims;
    uint64_t chunkSize;
    uint64_t tileSize;
};

