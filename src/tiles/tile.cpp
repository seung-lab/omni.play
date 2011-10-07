#include "chunks/segChunkDataInterface.hpp"
#include "chunks/chunkData.hpp"
#include "chunks/segChunk.h"
#include "system/cache/omCacheBase.h"
#include "tiles/cache/raw/omRawSegTileCacheTypes.hpp"
#include "tiles/channelTileFilter.hpp"
#include "tiles/omTextureID.h"
#include "tiles/tile.h"
#include "utility/dataWrappers.h"
#include "view2d/omView2dConverters.hpp"
#include "viewGroup/omViewGroupState.h"
#include "volume/mipVolume.h"

tile::tile(OmCacheBase* cache, const tileCoord& key)
    : cache_(cache)
    , key_(key)
    , tileLength_(key.getVolume()->Coords().GetChunkDimension())
    , mipChunkCoord_(tileToMipCoord())
{}

tile::~tile()
{}

void tile::LoadData()
{
    if(getVolType() == CHANNEL) {
        load8bitChannelTile();

    } else {
        load32bitSegmentationTile();
    }
}

int tile::getChunkSliceNum(){
    return getDepth() % (getVol()->Coords().GetChunkDimension());
}

void tile::load8bitChannelTile()
{
    channel* chan = reinterpret_cast<channel*>(getVol());
    chunk* chunk = chan->GetChunk(mipChunkCoord_);

    OmPooledTile<uint8_t>* tileData =
        chunk->Data()->ExtractDataSlice8bit(key_.getViewType(),
                                            getChunkSliceNum());

    channelTileFilter::Filter(tileData);

    texture_.reset(new OmTextureID(tileLength_, tileData));
}

void tile::load32bitSegmentationTile()
{
    segmentation* seg = reinterpret_cast<segmentation*>(getVol());
    segChunk* chunk = seg->GetChunk(mipChunkCoord_);

    PooledTile32Ptr imageData =
        chunk->SegData()->ExtractDataSlice32bit(key_.getViewType(),
                                                getChunkSliceNum());

    OmPooledTile<OmColorARGB>* colorMappedData =
        key_.getViewGroupState()->ColorTile(imageData->GetData(),
                                            tileLength_,
                                            key_);

    texture_.reset(new OmTextureID(tileLength_, colorMappedData));
}

coords::chunkCoord tile::tileToMipCoord(){
    return key_.getCoord().toDataCoord(key_.getVolume(), key_.getLevel()).toChunkCoord();
}

int tile::getDepth()
{
    const uint32_t factor = om::math::pow2int(key_.getLevel());

    return OmView2dConverters::GetViewTypeDepth(key_.getCoord(),
                                                key_.getViewType()) / factor;
}

ObjectType tile::getVolType() const {
    return getVol()->getVolumeType();
}

uint32_t tile::NumBytes() const
{
    if(getVolType() == CHANNEL) {
        return 128*128;
    }

    return 128*128*4;
}
