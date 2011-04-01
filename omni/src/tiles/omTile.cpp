#include "chunks/omChunkTypes.hpp"
#include "chunks/omSegChunk.h"
#include "system/cache/omCacheBase.h"
#include "tiles/omTextureID.h"
#include "tiles/omTile.h"
#include "utility/dataWrappers.h"
#include "utility/image/omFilterImage.hpp"
#include "utility/image/omImage.hpp"
#include "viewGroup/omViewGroupState.h"
#include "volume/omMipVolume.h"
#include "view2d/omView2dConverters.hpp"

OmTile::OmTile(OmCacheBase* cache, const OmTileCoord& key)
    : cache_(cache)
    , key_(key)
    , tileLength_(key.getVolume()->Coords().GetChunkDimension())
    , dims_(Vector2i(tileLength_, tileLength_))
    , mipChunkCoord_(tileToMipCoord())
{}

void OmTile::LoadData()
{
    if(isMipChunkCoordValid()){
        doLoadData();
    } else {
        makeNullTextureID();
    }
}

bool OmTile::isMipChunkCoordValid(){
    return getVol()->Coords().ContainsMipChunkCoord(mipChunkCoord_);
}

void OmTile::makeNullTextureID(){
    texture_ = boost::make_shared<OmTextureID>();
}

void OmTile::doLoadData()
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
    OmChunkPtr chunk;

    OmChannel* chan = reinterpret_cast<OmChannel*>(getVol());
    chan->GetChunk(chunk, mipChunkCoord_);

    OmImage<uint8_t, 2> slice =
        chunk->Data()->ExtractDataSlice8bit(key_.getViewType(),
                                            getChunkSliceNum());

    OmPooledTile<uint8_t>* vData = OmImageFilter::FilterChannel(slice);

    texture_ = boost::make_shared<OmTextureID>(dims_, vData);
}

void OmTile::load32bitSegmentationTile()
{
    OmSegChunkPtr chunk;

    OmSegmentation* seg = reinterpret_cast<OmSegmentation*>(getVol());
    seg->GetChunk(chunk, mipChunkCoord_);

    boost::shared_ptr<uint32_t> imageData =
        chunk->SegData()->ExtractDataSlice32bit(key_.getViewType(),
                                                getChunkSliceNum());

    OmPooledTile<OmColorARGB>* colorMappedData =
        key_.getViewGroupState()->ColorTile(imageData.get(),
                                            dims_,
                                            key_);

    texture_ = boost::make_shared<OmTextureID>(dims_, colorMappedData);
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

ObjectType OmTile::getVolType(){
    return getVol()->getVolumeType();
}

uint32_t OmTile::NumBytes() const
{
    if(!texture_){
        return 0;
    }
    return texture_->NumBytes();
}
