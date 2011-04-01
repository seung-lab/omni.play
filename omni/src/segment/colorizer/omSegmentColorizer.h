#ifndef OM_SEGEMNT_COLORIZER_H
#define OM_SEGEMNT_COLORIZER_H

#include "common/omCommon.h"
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
                       const Vector2i& dims,
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

#endif
