#include "chunks/omSegChunk.h"
#include "system/cache/omCacheBase.h"
#include "tiles/omTextureID.h"
#include "tiles/omTile.h"
#include "utility/dataWrappers.h"
#include "tiles/omChannelTileFilter.hpp"
#include "utility/image/omImage.hpp"
#include "viewGroup/omViewGroupState.h"
#include "volume/omMipVolume.h"
#include "view2d/omView2dConverters.hpp"

OmTile::OmTile(OmCacheBase* cache, const OmTileCoord& key)
    : cache_(cache)
    , key_(key)
    , tileLength_(key.getVolume()->Coords().GetChunkDimension())
    , mipChunkCoord_(tileToMipCoord())
{}

OmTile::~OmTile()
{}

void OmTile::LoadData()
{
    if(getVolType() == CHANNEL) {
        load8bitChannelTile();

    } else {
        load32bitSegmentationTile();
    }
}

int OmTile::getChunkSliceNum(){
    return getDepth() % (getVol()->Coords().GetChunkDimension());
}

void OmTile::load8bitChannelTile()
{
    OmChannel* chan = reinterpret_cast<OmChannel*>(getVol());
    OmChunk* chunk = chan->GetChunk(mipChunkCoord_);

    OmPooledTile<uint8_t>* tileData =
        chunk->Data()->ExtractDataSlice8bit(key_.getViewType(),
                                            getChunkSliceNum());

    OmChannelTileFilter::Filter(tileData);

    texture_.reset(new OmTextureID(tileLength_, tileData));
}

void OmTile::load32bitSegmentationTile()
{
    OmSegmentation* seg = reinterpret_cast<OmSegmentation*>(getVol());
    OmSegChunk* chunk = seg->GetChunk(mipChunkCoord_);

    PooledTile32Ptr imageData =
        chunk->SegData()->ExtractDataSlice32bit(key_.getViewType(),
                                                getChunkSliceNum());

    OmPooledTile<OmColorARGB>* colorMappedData =
        key_.getViewGroupState()->ColorTile(imageData->GetData(),
                                            tileLength_,
                                            key_);

    texture_.reset(new OmTextureID(tileLength_, colorMappedData));
}

OmChunkCoord OmTile::tileToMipCoord(){
    return getVol()->Coords().DataToMipCoord(key_.getDataCoord(), key_.getLevel());
}

int OmTile::getDepth()
{
    const uint32_t factor = om::pow2int(key_.getLevel());

    return OmView2dConverters::GetViewTypeDepth(key_.getDataCoord(),
                                                key_.getViewType()) / factor;
}

ObjectType OmTile::getVolType() const {
    return getVol()->getVolumeType();
}

uint32_t OmTile::NumBytes() const
{
    if(getVolType() == CHANNEL) {
        return 128*128;
    }

    return 128*128*4;
}
