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
                                       const Vector2i& dims,
                                       OmViewGroupState* vgs)
    : cacheSegments_(new OmCacheSegments(cache))
{
    freshness_.set(0);

    SegmentColorParams p = {
        sccType,
        dims.x * dims.y,
        vgs,
        cache,
        cacheSegments_.get()
    };

    params_ = p;
}

OmSegmentColorizer::~OmSegmentColorizer()
{}

void OmSegmentColorizer::setup()
{
    freshness_.set(OmCacheManager::GetFreshness());

    const OmSegID curSize = params_.segments->getMaxValue() + 1;

    cacheSegments_->CheckSizeAndMSTfreshness(curSize);

    if(curSize != colorCache_.Size()){
        colorCache_.Resize(curSize);
    }
}

boost::shared_ptr<OmColorRGBA>
OmSegmentColorizer::ColorTile(uint32_t const* imageData)
{
    setup();

    {
        //prevent vectors from being resized while we're reading
        zi::rwmutex::read_guard g(colorCache_);
        zi::rwmutex::read_guard g2(*cacheSegments_);

        boost::shared_ptr<OmColorRGBA> colorMappedDataPtr
            = OmSmartPtr<OmColorRGBA>::MallocNumElements(params_.numElements,
                                                         om::DONT_ZERO_FILL);

        // OmTimer timer;

        OmSegmentColorizerImpl c(params_, colorCache_, freshness_.get());
        c.ColorTile(imageData, colorMappedDataPtr.get());

        // timer.PrintV("done coloring tile");

        return colorMappedDataPtr;
    }
}
