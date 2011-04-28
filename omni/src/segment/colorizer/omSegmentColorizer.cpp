#include "tiles/pools/omPooledTile.hpp"
#include "segment/colorizer/omSegmentColorizer.h"
#include "segment/colorizer/omSegmentColorizerImpl.hpp"
#include "segment/colorizer/omSegmentColors.hpp"
#include "system/cache/omCacheManager.h"
#include "utility/omSmartPtr.hpp"
#include "utility/omTimer.hpp"

const std::vector<uint8_t> OmSegmentColorizer::SelectedColorLookupTable =
    OmSegmentColors::makeLookupTable();

OmSegmentColorizer::OmSegmentColorizer(OmSegments* cache,
                                       const OmSegmentColorCacheType sccType,
                                       const int tileDim,
                                       OmViewGroupState* vgs)
{
    freshness_.set(0);

    SegmentColorParams p = {
        sccType,
        tileDim * tileDim,
        vgs,
        cache,
    };

    params_ = p;
}

OmSegmentColorizer::~OmSegmentColorizer()
{}

void OmSegmentColorizer::setup()
{
    freshness_.set(OmCacheManager::GetFreshness());

    const OmSegID curSize = params_.segments->getMaxValue() + 1;

    if(curSize != colorCache_.Size()){
        colorCache_.Resize(curSize);
    }
}

OmPooledTile<OmColorARGB>*
OmSegmentColorizer::ColorTile(uint32_t const* imageData)
{
    setup();

    {
        //prevent vectors from being resized while we're reading
        zi::rwmutex::read_guard g(colorCache_);

        OmPooledTile<OmColorARGB>* colorMappedDataPtr
            = new OmPooledTile<OmColorARGB>();

        // OmTimer timer;

        OmSegmentColorizerImpl c(params_, colorCache_, freshness_.get());
        c.ColorTile(imageData, colorMappedDataPtr->GetData());

        // timer.PrintV("done coloring tile");

        return colorMappedDataPtr;
    }
}
