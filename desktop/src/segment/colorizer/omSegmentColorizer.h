#pragma once

#include "common/common.h"
#include "segment/colorizer/omSegmentColorizerColorCache.hpp"
#include "segment/colorizer/omSegmentColorizerTypes.h"
#include "utility/omLockedPODs.hpp"

class OmSegment;
class OmSegments;
template <class> class OmPooledTile;

class OmSegmentColorizer {
public:
    OmSegmentColorizer(OmSegments*,
                       const OmSegmentColorCacheType,
                       const int tileDim,
                       OmViewGroupState* vgs);

    ~OmSegmentColorizer();

    OmPooledTile<OmColorARGB>* ColorTile(uint32_t const*const imageData);

    static const std::vector<uint8_t> SelectedColorLookupTable;

private:
    OmSegmentColorizerColorCache colorCache_;

    SegmentColorParams params_;

    LockedUint64 freshness_;

    void setup();

    friend class SegmentTests1;
};

