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

int OmTile::getVolDepth(){
    return getDepth() % (getVol()->Coords().GetChunkDimension());
}

void OmTile::load8bitChannelTile()
{
    OmChunkPtr chunk;

    //TODO: get rid of wrapper... (purcaro)
    ChannelDataWrapper cdw(getVol()->getID());
    cdw.GetChannel().GetChunk(chunk, mipChunkCoord_);

    OmImage<uint8_t, 2> slice =
        chunk->Data()->ExtractDataSlice8bit(key_.getViewType(),
                                            getVolDepth());

    boost::shared_ptr<uint8_t> vData = OmImageFilter::FilterChannel(slice);

    texture_ = boost::make_shared<OmTextureID>(dims_, vData);
}

void OmTile::load32bitSegmentationTile()
{
    OmSegChunkPtr chunk;

    //TODO: get rid of wrapper... (purcaro)
    SegmentationDataWrapper sdw(getVol()->getID());
    sdw.GetSegmentation().GetChunk(chunk, mipChunkCoord_);

    boost::shared_ptr<uint32_t> imageData =
        chunk->SegData()->ExtractDataSlice32bit(key_.getViewType(),
                                                getVolDepth());

    boost::shared_ptr<OmColorRGBA> colorMappedData =
        key_.getViewGroupState()->ColorTile(imageData.get(),
                                            dims_,
                                            key_);

    texture_ = boost::make_shared<OmTextureID>(dims_, colorMappedData);
}

OmChunkCoord OmTile::tileToMipCoord()
{
    // find mip coord
    NormCoord normCoord = getVol()->Coords().DataToNormCoord(key_.getDataCoord());
    return getVol()->Coords().NormToMipCoord(normCoord, key_.getLevel());
}

int OmTile::getDepth()
{
    const NormCoord normCoord = getVol()->Coords().DataToNormCoord(key_.getDataCoord());
    const DataCoord dataCoord = getVol()->Coords().NormToDataCoord(normCoord);
    const float factor = om::pow2int(key_.getLevel());

    switch(key_.getViewType()){
    case XY_VIEW:
        return (int)(dataCoord.z/factor);
    case XZ_VIEW:
        return (int)(dataCoord.y/factor);
    case YZ_VIEW:
        return (int)(dataCoord.x/factor);
    default:
        throw OmArgException("invalid viewType");
    }
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
