#include "system/viewGroup/omViewGroupState.h"
#include "view2d/omTextureID.h"
#include "tiles/omTile.h"
#include "volume/omMipChunk.h"
#include "volume/omMipVolume.h"

#include <boost/make_shared.hpp>

OmTile::OmTile(OmCacheBase* cache, const OmTileCoord& key)
	: cache_(cache)
	, key_(key)
	, tileLength_(key.getVolume()->GetChunkDimension())
	, dims_(Vector2i(tileLength_, tileLength_))
	, mipChunkCoord_(TileToMipCoord())
{
}

OmTextureIDPtr OmTile::loadData()
{
	if(isMipChunkCoordValid()){
		doLoadData();
	} else {
		makeNullTextureID();
	}

	return texture_;
}

bool OmTile::isMipChunkCoordValid(){
	return getVol()->ContainsMipChunkCoord(mipChunkCoord_);
}

void OmTile::makeNullTextureID(){
	texture_ = boost::make_shared<OmTextureID>();
}

void OmTile::doLoadData()
{
	texture_ = boost::make_shared<OmTextureID>(dims_, cache_);

	if(getVolType() == CHANNEL) {
		boost::shared_ptr<uint8_t> vData = GetImageData8bit();
		texture_->setData(vData);
	} else {
		boost::shared_ptr<uint32_t> imageData = GetImageData32bit();
		boost::shared_ptr<OmColorRGBA> colorMappedData =
			key_.getViewGroupState()->ColorTile(imageData,
							    dims_,
							    key_);
		texture_->setData(colorMappedData);
	}
}

int OmTile::getVolDepth(){
	return GetDepth() % (getVol()->GetChunkDimension());
}

boost::shared_ptr<uint8_t> OmTile::GetImageData8bit()
{
	OmMipChunkPtr chunk;
	getVol()->GetChunk(chunk, mipChunkCoord_, true);

	return chunk->ExtractDataSlice8bit(key_.getViewType(),
					   getVolDepth());
}

boost::shared_ptr<uint32_t> OmTile::GetImageData32bit()
{
	OmMipChunkPtr chunk;
	getVol()->GetChunk(chunk, mipChunkCoord_, true);

	return chunk->ExtractDataSlice32bit(key_.getViewType(),
					    getVolDepth());
}

OmMipChunkCoord OmTile::TileToMipCoord()
{
	// find mip coord
	NormCoord normCoord = getVol()->SpaceToNormCoord(key_.getSpaceCoord());
	return getVol()->NormToMipCoord(normCoord, key_.getLevel());
}

int OmTile::GetDepth()
{
	const NormCoord normCoord = getVol()->SpaceToNormCoord(key_.getSpaceCoord());
	const DataCoord dataCoord = getVol()->NormToDataCoord(normCoord);
        const float factor = OMPOW(2, key_.getLevel());

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
