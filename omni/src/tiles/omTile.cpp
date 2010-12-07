#include "system/cache/omCacheBase.h"
#include "tiles/omTextureID.h"
#include "tiles/omTile.h"
#include "utility/image/omFilterImage.hpp"
#include "viewGroup/omViewGroupState.h"
#include "volume/omMipChunk.h"
#include "volume/omMipVolume.h"

OmTile::OmTile(OmCacheBase* cache, const OmTileCoord& key)
	: cache_(cache)
	, key_(key)
	, tileLength_(key.getVolume()->GetChunkDimension())
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
	return getVol()->ContainsMipChunkCoord(mipChunkCoord_);
}

void OmTile::makeNullTextureID(){
	texture_ = boost::make_shared<OmTextureID>();
}

void OmTile::doLoadData()
{
	if(getVolType() == CHANNEL) {
		OmImage<uint8_t, 2> slice = getImageData8bit();
		boost::shared_ptr<uint8_t> vData = OmImageFilter::FilterChannel(slice);
		texture_ = boost::make_shared<OmTextureID>(dims_, vData);

	} else {
		boost::shared_ptr<uint32_t> imageData = getImageData32bit();
		boost::shared_ptr<OmColorRGBA> colorMappedData =
			key_.getViewGroupState()->ColorTile(imageData.get(),
												dims_,
												key_);
		texture_ = boost::make_shared<OmTextureID>(dims_, colorMappedData);
	}

	cache_->UpdateSize(texture_->NumBytes());
}

int OmTile::getVolDepth(){
	return getDepth() % (getVol()->GetChunkDimension());
}

OmImage<uint8_t, 2> OmTile::getImageData8bit()
{
	OmMipChunkPtr chunk;
	getVol()->GetChunk(chunk, mipChunkCoord_);

	return chunk->ExtractDataSlice8bit(key_.getViewType(),
									   getVolDepth());
}

boost::shared_ptr<uint32_t> OmTile::getImageData32bit()
{
	OmMipChunkPtr chunk;
	getVol()->GetChunk(chunk, mipChunkCoord_);

	return chunk->ExtractDataSlice32bit(key_.getViewType(),
										getVolDepth());
}

OmMipChunkCoord OmTile::tileToMipCoord()
{
	// find mip coord
	NormCoord normCoord = getVol()->SpaceToNormCoord(key_.getSpaceCoord());
	return getVol()->NormToMipCoord(normCoord, key_.getLevel());
}

int OmTile::getDepth()
{
	const NormCoord normCoord = getVol()->SpaceToNormCoord(key_.getSpaceCoord());
	const DataCoord dataCoord = getVol()->NormToDataCoord(normCoord);
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
